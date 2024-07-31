#include <broadcast_socket.h>
#include "freertos/FreeRTOS.h"
#include "esp_netif.h"
#include "socket.h"
#include "esp_log.h"
#include "lvgl_ui.h"
#include "data.h"
#define UDP_PORT 3333

#define MULTICAST_TTL 1
#define MULTICAST_IPV4_ADDR "232.10.11.12"

int sock;

char* TAG_BC = "broadcast";

#define STATUS_MESSAGE_HEADER "STATUS|"

void handle_stint(char* saveptr) {
    bool enabled = strcmp(strtok_r(NULL, ";", &saveptr), "true") == 0;
    long target = atol(strtok_r(NULL, ";", &saveptr));
    long elapsed = atol(strtok_r(NULL, ";", &saveptr));
    ESP_LOGI(TAG_BC, "Stint: %d - %ld - %ld", enabled, target, elapsed);
    
    data()->stint.currently_running = enabled;
    data()->stint.elapsed = elapsed;
    data()->stint.target = target;
}

void handle_lapno(char* saveptr) {
    int lap_no = atoi(strtok_r(NULL, ";", &saveptr));
    data()->lap_data.lap_no = lap_no;
}

void handle_lap(char* saveptr) {
    long lapno = atol(strtok_r(NULL, ";", &saveptr));
    long laptime = atol(strtok_r(NULL, ";", &saveptr));
    switch (lapno) {
        case 0: 
            data()->lap_data.current_lap = laptime;
        break;
        case -1: 
            data()->lap_data.best_lap = laptime;
        break;
        default: 
            data()->lap_data.last_laps[lapno-1].lap_no = lapno;
            data()->lap_data.last_laps[lapno-1].lap_time_ms = laptime;
        break;

    }
}

void handle_temp_pres(char* saveptr, struct car_sensor* sensor) {
    long temp = atol(strtok_r(NULL, ";", &saveptr));
    float pres = atof(strtok_r(NULL, ";", &saveptr));
    sensor->preassure = pres;
    sensor->temp = temp;
}

void parse_message(char* message) {
    if (strncmp(STATUS_MESSAGE_HEADER, message, strlen(STATUS_MESSAGE_HEADER)) != 0) {
        ESP_LOGI(TAG_BC, "wrong message, skip");
        return;
    }

    char *token, *subtoken;
    char *saveptr1, *saveptr2;

    for (token = strtok_r(message, "|", &saveptr1);
         token != NULL;
         token = strtok_r(NULL, "|", &saveptr1)) {   
                
        subtoken = strtok_r(token, ";", &saveptr2);
        if(strcmp(subtoken, "STNT") == 0) {
            handle_stint(saveptr2);
        } else if (strcmp(subtoken, "LAP-NO") == 0) {
            handle_lapno(saveptr2); 
        } else if (strcmp(subtoken, "WATER") == 0) {
            handle_temp_pres(saveptr2, &data()->water);
        } else if (strcmp(subtoken, "OIL") == 0) {
            handle_temp_pres(saveptr2,  &data()->oil);
        } else if (strcmp(subtoken, "GAS") == 0) {
            handle_temp_pres(saveptr2, &data()->gas);
        }else if (strcmp(subtoken, "OIL_WARN") == 0) {
            handle_temp_pres(saveptr2, oil_warn());
        }else if (strcmp(subtoken, "WATER_WARN") == 0) {
            handle_temp_pres(saveptr2, water_warn());
        }
    }

    gptimer_handle_t gp_timer = data()->stint.gptimer;
    if(data()->stint.currently_running && !data()->stint.gptimer_running) {
        gptimer_start(gp_timer);
        data()->stint.gptimer_running = true;
    } else if (!data()->stint.currently_running && data()->stint.gptimer_running) {
        gptimer_stop(gp_timer);
        data()->stint.gptimer_running = false;
    }

    gptimer_set_raw_count(gp_timer, data()->stint.elapsed * 1000);
    ESP_LOGI(TAG_BC, "finished parsing");
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
                    char recvbuf[500];
                    char raddr_name[32] = { 0 };
                    struct sockaddr_storage raddr; // Large enough for both IPv4 or IPv6
                    socklen_t socklen = sizeof(raddr);
                    int len = recvfrom(sock, recvbuf, sizeof(recvbuf)-1, 0,
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

                    recvbuf[len] = 0; // Null-terminate whatever we received and treat like a string...
                    ESP_LOGI(TAG_BC,"%s\n", recvbuf);

                    parse_message(recvbuf);
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