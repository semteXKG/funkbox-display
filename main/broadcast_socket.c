#include <broadcast_socket.h>
#include "freertos/FreeRTOS.h"
#include "esp_netif.h"
#include "socket.h"
#include "esp_log.h"
#include "lvgl_ui.h"
#include "data.h"
#include "esp_timer.h"

#define UDP_PORT 3333

#define MULTICAST_TTL 1
#define MULTICAST_IPV4_ADDR "232.10.11.12"
#define HEADER_LENGTH 2
int sock;

char* TAG_BC = "broadcast";

bool first_message = true;

void print_data(struct mcu_data* data) {
    ESP_LOGI(TAG_BC, "Time Adjust: [%ld]", data->network_time_adjustment);
    ESP_LOGI(TAG_BC, "LapNo [%d], Best [%ld], Current [%ld]", data->lap_data.lap_no, data->lap_data.best_lap, data->lap_data.current_lap);
    for (int i = 0; i < 5; i++) {
        ESP_LOGI(TAG_BC, "[%d] LapNo [%d], Time [%"PRId64"]", i, data->lap_data.last_laps[i].lap_no, data->lap_data.last_laps[i].lap_time_ms);
    }

    ESP_LOGI(TAG_BC, "incoming command cnt: [%d]", data->incoming_commands_last_idx);
    for (int i = 0; i < 5; i++) {
        ESP_LOGI(TAG_BC, "Type [%d] Created [%"PRId64"], Handled [%"PRId64"]", data->incoming_commands[i].type, data->incoming_commands[i].created, data->incoming_commands[i].handled);
    }
    
    ESP_LOGI(TAG_BC, "incoming command cnt: [%d]", data->incoming_commands_last_idx);
    for (int i = 0; i < 5; i++) {
      ESP_LOGI(TAG_BC, "Type [%d] Created [%"PRId64"], Handled [%"PRId64"]", data->incoming_commands[i].type, data->incoming_commands[i].created, data->incoming_commands[i].handled);
    }

    ESP_LOGI(TAG_BC, "outgoing command cnt: [%d]", data->outgoing_commands_last_idx);
    for (int i = 0; i < 5; i++) {
      ESP_LOGI(TAG_BC, "Type [%d] Created [%"PRId64"], Handled [%"PRId64"]", data->outgoing_commands[i].type, data->outgoing_commands[i].created, data->outgoing_commands[i].handled);
    }

    ESP_LOGI(TAG_BC, "events cnt: [%d]", data->events_cnt);
    for (int i = 0; i < 5; i++) {
      ESP_LOGI(TAG_BC, "[%d] Type [%d] Created [%"PRId64"] Displayed_Since [%"PRId64"]", i, data->events[i].type, data->events[i].created_at, data->events[i].displayed_since);
    }
}

struct event* find_old_entry(struct event* old_events, int id) {
    for (int i = 0; i < 5; i++) {
        if (old_events[i].id == id) {
            return &old_events[i];
        }
    }
    return NULL;
}

void transfer_event_data(struct mcu_data* old, struct mcu_data* new) {
    for (int i = 0; i < 5; i++) {
        struct event* old_data = find_old_entry(old->events, new->events[i].id);
        new->events[i].displayed_since = old_data != NULL ? old_data->displayed_since : (first_message ? 1 : 0);        
    
        if (new->events[i].type ==  EVT_TIME_REMAIN) {
            // ignore last timestamp    
            struct time_str ll = convert_millis_to_time(new->stint.target - new->stint.elapsed);
            if (ll.minutes != 0) {
                sprintf(new->events[i].text, "%d min\nLEFT", ll.minutes);
            } else {
                sprintf(new->events[i].text, "%d sec\nLEFT", ll.seconds);
            }
        } else if (new->events[i].type ==  EVT_LAP) {
            int lap_number = atoi(strtok(new->events[i].text, ";"));
            int lap_time_ms = atol(strtok(NULL, ";"));
            int lap_diff_ms = atol(strtok(NULL, ";"));

            new->events[i].severity = ((lap_diff_ms < 0) ? POSITIVE : CRIT);

            struct time_str ll = convert_millis_to_time(lap_time_ms);
            struct time_str ll_diff = convert_millis_to_time(lap_diff_ms);

            sprintf(new->events[i].text, 
                        "Lap %d\n%1d:%02d.%02d\n%s%1d.%02d", 
                        lap_number,
                        ll.minutes, ll.seconds, ll.milliseconds/10,
                        (lap_diff_ms < 0 ? "-":"+"), ll_diff.seconds + (ll_diff.minutes * 60), ll_diff.milliseconds/10);
        } else if (new->events[i].type == EVT_STATE_CHANGE) {
            char* target = strtok(new->events[i].text,";");
            char* state = strtok(NULL, ";");
            new->events[i].severity = NORMAL;
            sprintf(new->events[i].text, "%s\n%s", target, state);
        }
    }
}


void parse_binary(char* message) {
    memcpy(get_inactive_data(), message, sizeof(struct mcu_data));
    transfer_event_data(get_data(), get_inactive_data());
    get_inactive_data()->network_time_adjustment = esp_timer_get_time() / 1000 - get_inactive_data()->network_time_adjustment;
    
    //print_data(get_inactive_data());
    data_switch_active();
    first_message = false;
}

/* Add a socket to the IPV4 multicast group */
static int socket_add_ipv4_multicast_group(esp_netif_t* interface, int sock, bool assign_source_if)
{
    struct ip_mreq imreq = { 0 };
    struct in_addr iaddr = { 0 };
    int err = 0;
    // Configure source interface

    esp_netif_ip_info_t ip_info = { 0 };
    err = esp_netif_get_ip_info(interface, &ip_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_BC, "Failed to get IP address info. Error 0x%x\n", err);
        goto err;
    }
    inet_addr_from_ip4addr(&iaddr, &ip_info.ip);

    // Configure multicast address to listen to
    err = inet_aton(MULTICAST_IPV4_ADDR, &imreq.imr_multiaddr.s_addr);
    if (err != 1) {
        ESP_LOGE(TAG_BC,"Configured IPV4 multicast address '%s' is invalid.\n", MULTICAST_IPV4_ADDR);
        // Errors in the return value have to be negative
        err = -1;
        goto err;
    }
    ESP_LOGE(TAG_BC,"Configured IPV4 Multicast address %s\n", inet_ntoa(imreq.imr_multiaddr.s_addr));
    if (!IP_MULTICAST(ntohl(imreq.imr_multiaddr.s_addr))) {
        ESP_LOGE(TAG_BC,"Configured IPV4 multicast address '%s' is not a valid multicast address. This will probably not work.\n", MULTICAST_IPV4_ADDR);
    }

    if (assign_source_if) {
        // Assign the IPv4 multicast source interface, via its IP
        // (only necessary if this socket is IPV4 only)
        err = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &iaddr,
                         sizeof(struct in_addr));
        if (err < 0) {
            ESP_LOGE(TAG_BC,"Failed to set IP_MULTICAST_IF. Error %d\n", errno);
            goto err;
        }
    }

    err = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         &imreq, sizeof(struct ip_mreq));
    if (err < 0) {
        ESP_LOGE(TAG_BC,"Failed to set IP_ADD_MEMBERSHIP. Error %d\n", errno);
        goto err;
    }

 err:
    return err;
}


static int create_multicast_ipv4_socket(esp_netif_t* interface)
{
    struct sockaddr_in saddr = { 0 };
    int sock = -1;
    int err = 0;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG_BC,"Failed to create socket. Error %d\n", errno);
        return -1;
    }

    // Bind the socket to any address
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(UDP_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

    uint8_t ttl = MULTICAST_TTL;
    if (err < 0) {
        ESP_LOGE(TAG_BC,"Failed to bind socket. Error %d\n", errno);
        goto err;
    }


    // Assign multicast TTL (set separately from normal interface TTL)
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(uint8_t));
    if (err < 0) {
        ESP_LOGE(TAG_BC,"Failed to set IP_MULTICAST_TTL. Error %d\n", errno);
        goto err;
    }

    // this is also a listening socket, so add it to the multicast
    // group for listening...
    err = socket_add_ipv4_multicast_group(interface, sock, true);
    if (err < 0) {
        goto err;
    }

    // All set, socket is configured for sending and receiving
    return sock;

err:
    close(sock);
    return -1;
}


void listen_broadcast(void *pvParameters)
{
    esp_netif_t* interface = (esp_netif_t*) pvParameters;
    while (1) {
        sock = create_multicast_ipv4_socket(interface);
        if (sock < 0) {
           //ESP_LOGE(TAG_BC,"Failed to create IPv4 multicast socket\n");
        }

        if (sock < 0) {
            // Nothing to do!
            vTaskDelay(5 / portTICK_PERIOD_MS);
            continue;
        }

        // set destination multicast addresses for sending from these sockets
        struct sockaddr_in sdestv4 = {
            .sin_family = PF_INET,
            .sin_port = htons(UDP_PORT),
        };
        // We know this inet_aton will pass because we did it above already
        inet_aton(MULTICAST_IPV4_ADDR, &sdestv4.sin_addr.s_addr);

        // Loop waiting for UDP received, and sending UDP packets if we don't
        // see any.
        int err = 1;
        while (err > 0) {
            struct timeval tv = {
                .tv_sec = 2,
                .tv_usec = 0,
            };
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(sock, &rfds);

            int s = select(sock + 1, &rfds, NULL, NULL, &tv);
            if (s < 0) {
                ESP_LOGE(TAG_BC,"Select failed: errno %d\n", errno);
                err = -1;
                break;
            } else if (s > 0) {
                if (FD_ISSET(sock, &rfds)) {
                    // Incoming daTAG_BCram received
                    char recvbuf[sizeof(struct mcu_data)+HEADER_LENGTH];
                    char raddr_name[32] = { 0 };
                    struct sockaddr_storage raddr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(raddr);
                    int len = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
                                       (struct sockaddr *)&raddr, &socklen);
                    if (len < 0) {
                        ESP_LOGE(TAG_BC,"multicast recvfrom failed: errno %d\n", errno);
                        err = -1;
                        break;
                    }

                    if (raddr.ss_family == PF_INET) {
                        inet_ntoa_r(((struct sockaddr_in *)&raddr)->sin_addr,
                                    raddr_name, sizeof(raddr_name)-1);
                    }
                    ESP_LOGI(TAG_BC,"received %d bytes from %s: ", len, raddr_name);
                    if(recvbuf[0] == 'S' && recvbuf[1] == 'T') {
                        parse_binary(recvbuf+2);
                    } else {
                        ESP_LOGI(TAG_BC, "Skipping unknown header");
                    }
                    ESP_LOGI(TAG_BC,"Done");
                }
            }
        }

        ESP_LOGE(TAG_BC,"Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
    }

}


void broadcast_socket_start() {
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  xTaskCreate(&listen_broadcast, "listen_task", 4096, netif, 5, NULL);
}