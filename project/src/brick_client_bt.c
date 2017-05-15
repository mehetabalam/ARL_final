#include "headers/brick_client_bt.h"
#include <stdlib.h>
#include <unistd.h>

size_t brick_bt_scan_devices (brick_bt_device_t *devices[MAX_BT_DEVICES])
{
  inquiry_info *scan_res = NULL;
  size_t num_rsp;
  int dev_id, sock, flags;
  int i;
  char addr[19] = { 0 };
  char name[248] = { 0 };

  flags = IREQ_CACHE_FLUSH;

  // Initializing BT Device for scan
  dev_id = hci_get_route(NULL);
  sock = hci_open_dev( dev_id );

  if (dev_id < 0 || sock < 0) {
    perror("opening socket");
    return -1;
  }

  memset(devices, 0, MAX_BT_DEVICES*sizeof(brick_bt_device_t*));

  // Allocate memory for all the devices found
  scan_res = (inquiry_info*)malloc(MAX_BT_DEVICES*sizeof(inquiry_info));

  // Scan to find all the devices in range
  num_rsp = hci_inquiry(dev_id, BT_INQUIRY_LEN, MAX_BT_DEVICES, NULL,
			&scan_res, flags);
  if( num_rsp < 0 ) {
    perror("hci_inquiry");
    return -1;
  }

  // For each of the found devices we retrieve its name
  for (i = 0; i < num_rsp; i++) {
    ba2str(&scan_res[i].bdaddr, addr);
    memset(name, 0, sizeof(name));
    if (hci_read_remote_name(sock, &scan_res[i].bdaddr, sizeof(name), name, 0) < 0) {
      strcpy(name, "[unknown]");
    };
    devices[i] = malloc(sizeof(brick_bt_device_t));
    strcpy(devices[i]->name, name);
    memcpy(&devices[i]->mac, &scan_res[i].bdaddr, sizeof(bdaddr_t));
  }

  free(scan_res);
  close(sock);
  return num_rsp;
}

int brick_bt_connect_device(int *brick_socket, bdaddr_t mac_addr)
{
  int connect_status;
  struct sockaddr_rc addr_data = { 0 };

  /* AF_BLUETOOTH means that it's a Bluetooth socket, while BTPROTO_RFCOMM
   * means that we are going to use the RFCOMM protocol to get the data.
   */
  *brick_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  /* As written on the official guide for the Bluetooth Libraries...
   * struct sockaddr_rc {
   *    sa_family_t	rc_family;
   *    bdaddr_t	    rc_bdaddr;
   *    uint8_t		rc_channel;
   *  };
   * The sockaddr_rc contains various and interesting values. The
   * communication family, the RFCOMM channel and the MAC address
   */
  addr_data.rc_family = AF_BLUETOOTH;
  addr_data.rc_channel = (uint8_t)1;
  addr_data.rc_bdaddr = mac_addr;

  // Let's connect to the brick
  connect_status = connect(*brick_socket, (struct sockaddr *)&addr_data, sizeof(addr_data));

  return connect_status;
}

ssize_t brick_bt_send(int brick_socket, const uint8_t data[], size_t size)
{
  size_t sent_bytes = 0;
  ssize_t res;

  do
  {
    res = send(brick_socket, data+sent_bytes, size-sent_bytes, 0);
    if (res < 0)
    {
        return res;
    }
    sent_bytes += res;
  }
  while (sent_bytes < MAX_DATA_SIZE*sizeof(uint8_t));

  return res;
}

ssize_t brick_bt_recv(int brick_socket, uint8_t incoming_data[], size_t size)
{
  size_t recv_bytes = 0;
  ssize_t res;

  do
  {
    res = recv(brick_socket, incoming_data+recv_bytes, size-recv_bytes, 0);
    if (res < 0)
    {
        return res;
    }
    recv_bytes += res;
  }
  while (recv_bytes < size);

  return res;
}

int brick_bt_close_connection(int brick_socket)
{
  return close(brick_socket);
}
