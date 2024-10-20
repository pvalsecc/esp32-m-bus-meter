#pragma once

#include <esp_zigbee_type.h>

void zigbee_create_indentify_cluster(struct esp_zb_cluster_list_s *clusterList);
void zigbee_create_basic_cluster(struct esp_zb_cluster_list_s *clusterList, const char *modelName);
