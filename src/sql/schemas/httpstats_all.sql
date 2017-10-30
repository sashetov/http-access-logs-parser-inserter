DROP DATABASE IF EXISTS httpstats_domains;
DROP DATABASE IF EXISTS httpstats_pages;
DROP DATABASE IF EXISTS httpstats_clients;
DROP DATABASE IF EXISTS httpstats_api;
DROP DATABASE IF EXISTS httpstats_jobs;
CREATE DATABASE httpstats_domains CHARACTER SET utf8 COLLATE utf8_bin;
USE httpstats_domains;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE domains ( id INT(11) NOT NULL, name VARCHAR(255) NOT NULL, alias_of INT NOT NULL, uid INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `name_a_u` (`name`, `alias_of`, `uid` ) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_totals ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `domains_count` ( `domains_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE DATABASE httpstats_clients CHARACTER SET utf8 COLLATE utf8_bin;
USE httpstats_clients;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE ips               ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, ipv4 INT UNSIGNED NOT NULL UNIQUE ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations         ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(3) NOT NULL UNIQUE) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices           ( id INT NOT NULL AUTO_INCREMENT, name VARCHAR(255) NOT NULL, version VARCHAR(255) NOT NULL,  UNIQUE KEY `id` (`id`),  UNIQUE  KEY `name_version_uniq` (`name`,`version`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers          ( id INT NOT NULL AUTO_INCREMENT, name VARCHAR(255) NOT NULL, version VARCHAR(255) NOT NULL,  UNIQUE KEY `id` (`id`),  UNIQUE  KEY `name_version_uniq` (`name`,`version`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses              ( id INT NOT NULL AUTO_INCREMENT, name VARCHAR(255) NOT NULL, version VARCHAR(255) NOT NULL,  UNIQUE KEY `id` (`id`),  UNIQUE  KEY `name_version_uniq` (`name`,`version`) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE visits_hourly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`),      UNIQUE KEY `id` (`id`),         UNIQUE KEY `date_domains_ip_count` ( `date`, `domains_id`, `ip_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (ip_id) REFERENCES ips(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations_hourly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, country_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_location_count` ( `date`, `domains_id`, `country_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (country_id) REFERENCES locations(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_hourly   ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, browser_id INT NOT NULL,  count INT NOT NULL, KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (browser_id) REFERENCES browsers(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices_hourly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, device_id INT NOT NULL,  count INT NOT NULL,  KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_devices_count` ( `date`, `domains_id`, `device_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (device_id) REFERENCES devices(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses_hourly       ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, os_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_os_count` ( `date`, `domains_id`, `os_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (os_id) REFERENCES oses(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE visits_daily     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`),      UNIQUE KEY `id` (`id`),         UNIQUE KEY `date_domains_ip_count` ( `date`, `domains_id`, `ip_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (ip_id) REFERENCES ips(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations_daily  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, country_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_location_count` ( `date`, `domains_id`, `country_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (country_id) REFERENCES locations(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_daily   ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, browser_id INT NOT NULL,  count INT NOT NULL, KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (browser_id) REFERENCES browsers(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices_daily    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, device_id INT NOT NULL,  count INT NOT NULL,  KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_devices_count` ( `date`, `domains_id`, `device_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (device_id) REFERENCES devices(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses_daily       ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, os_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_os_count` ( `date`, `domains_id`, `os_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (os_id) REFERENCES oses(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE visits_weekly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`),      UNIQUE KEY `id` (`id`),         UNIQUE KEY `date_domains_ip_count` ( `date`, `domains_id`, `ip_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (ip_id) REFERENCES ips(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations_weekly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, country_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_location_count` ( `date`, `domains_id`, `country_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (country_id) REFERENCES locations(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_weekly   ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, browser_id INT NOT NULL,  count INT NOT NULL, KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (browser_id) REFERENCES browsers(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices_weekly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, device_id INT NOT NULL,  count INT NOT NULL,  KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_devices_count` ( `date`, `domains_id`, `device_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (device_id) REFERENCES devices(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses_weekly       ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, os_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_os_count` ( `date`, `domains_id`, `os_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (os_id) REFERENCES oses(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE visits_monthly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`),      UNIQUE KEY `id` (`id`),         UNIQUE KEY `date_domains_ip_count` ( `date`, `domains_id`, `ip_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (ip_id) REFERENCES ips(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations_monthly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, country_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_location_count` ( `date`, `domains_id`, `country_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (country_id) REFERENCES locations(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_monthly   ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, browser_id INT NOT NULL,  count INT NOT NULL, KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (browser_id) REFERENCES browsers(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices_monthly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, device_id INT NOT NULL,  count INT NOT NULL,  KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_devices_count` ( `date`, `domains_id`, `device_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (device_id) REFERENCES devices(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses_monthly       ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, os_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_os_count` ( `date`, `domains_id`, `os_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (os_id) REFERENCES oses(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE visits_yearly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`),      UNIQUE KEY `id` (`id`),         UNIQUE KEY `date_domains_ip_count` ( `date`, `domains_id`, `ip_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (ip_id) REFERENCES ips(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations_yearly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, country_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_location_count` ( `date`, `domains_id`, `country_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (country_id) REFERENCES locations(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_yearly   ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, browser_id INT NOT NULL,  count INT NOT NULL, KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (browser_id) REFERENCES browsers(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices_yearly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, device_id INT NOT NULL,  count INT NOT NULL,  KEY `date` (`date`),  UNIQUE KEY `id` (`id`),   UNIQUE KEY `date_domains_devices_count` ( `date`, `domains_id`, `device_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (device_id) REFERENCES devices(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses_yearly       ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT(11) NOT NULL, os_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_os_count` ( `date`, `domains_id`, `os_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (os_id) REFERENCES oses(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE visits_total     ( id INT NOT NULL AUTO_INCREMENT, domains_id INT(11) NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `domains_ip_count` ( `domains_id`, `ip_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (ip_id) REFERENCES ips(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE locations_total  ( id INT NOT NULL AUTO_INCREMENT, domains_id INT(11) NOT NULL, country_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `domains_location_count` ( `domains_id`, `country_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (country_id) REFERENCES locations(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_total   ( id INT NOT NULL AUTO_INCREMENT, domains_id INT(11) NOT NULL, browser_id INT NOT NULL,  count INT NOT NULL, UNIQUE KEY `id` (`id`),   UNIQUE KEY `domains_browser_count` ( `domains_id`, `browser_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (browser_id) REFERENCES browsers(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE devices_total    ( id INT NOT NULL AUTO_INCREMENT, domains_id INT(11) NOT NULL, device_id INT NOT NULL,  count INT NOT NULL,  UNIQUE KEY `id` (`id`),   UNIQUE KEY `domains_devices_count` ( `domains_id`, `device_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (device_id) REFERENCES devices(id) )  ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE oses_total       ( id INT NOT NULL AUTO_INCREMENT, domains_id INT(11) NOT NULL, os_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `domains_os_count` ( `domains_id`, `os_id`, `count` ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (os_id) REFERENCES oses(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE DATABASE httpstats_pages CHARACTER SET utf8 COLLATE utf8_bin;
USE httpstats_pages;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE pages_paths_full ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(500) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL UNIQUE ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvectors_inn ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, a_id INT NOT NULL, b_id INT NOT NULL, UNIQUE KEY `tvector_inn` (`a_id`, `b_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE external_domains ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255) NOT NULL, UNIQUE KEY `name` (`name`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(500) NOT NULL, search_engine_id INT NOT NULL, UNIQUE KEY `name_sid` (`name`,`search_engine_id`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvectors_inc ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, referer_domain_id INT NOT NULL, referer_page_id INT NOT NULL, page_id INT NOT NULL,UNIQUE KEY `tvector_inc` (`referer_domain_id`,`referer_page_id`,`page_id`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16; 
CREATE TABLE pageviews_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY pageviews_key ( date, domains_id, page_id, ip_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (ip_id) REFERENCES httpstats_clients.ips (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, size_kb INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY bandwidth_key ( date, domains_id, page_id, size_kb ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinn_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_internal_key (date, tvinn_id, count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinn_id) REFERENCES tvectors_inn (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY referers_key ( date, domains_id, page_id, referer_domain_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (referer_domain_id) REFERENCES external_domains (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY search_terms_key ( date, domains_id, page_id, search_term_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (search_term_id) REFERENCES search_terms (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinc_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_incoming_key (date, domains_id, tvinc_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinc_id) REFERENCES tvectors_inc (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE pageviews_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY pageviews_key ( date, domains_id, page_id, ip_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (ip_id) REFERENCES httpstats_clients.ips (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, size_kb INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY bandwidth_key ( date, domains_id, page_id, size_kb ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinn_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_internal_key (date, tvinn_id, count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinn_id) REFERENCES tvectors_inn (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY referers_key ( date, domains_id, page_id, referer_domain_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (referer_domain_id) REFERENCES external_domains (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY search_terms_key ( date, domains_id, page_id, search_term_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (search_term_id) REFERENCES search_terms (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinc_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_incoming_key (date, domains_id, tvinc_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinc_id) REFERENCES tvectors_inc (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE pageviews_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY pageviews_key ( date, domains_id, page_id, ip_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (ip_id) REFERENCES httpstats_clients.ips (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, size_kb INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY bandwidth_key ( date, domains_id, page_id, size_kb ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinn_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_internal_key (date, tvinn_id, count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinn_id) REFERENCES tvectors_inn (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY referers_key ( date, domains_id, page_id, referer_domain_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (referer_domain_id) REFERENCES external_domains (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY search_terms_key ( date, domains_id, page_id, search_term_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (search_term_id) REFERENCES search_terms (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinc_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_incoming_key (date, domains_id, tvinc_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinc_id) REFERENCES tvectors_inc (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE pageviews_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY pageviews_key ( date, domains_id, page_id, ip_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (ip_id) REFERENCES httpstats_clients.ips (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, size_kb INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY bandwidth_key ( date, domains_id, page_id, size_kb ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinn_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_internal_key (date, tvinn_id, count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinn_id) REFERENCES tvectors_inn (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY referers_key ( date, domains_id, page_id, referer_domain_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (referer_domain_id) REFERENCES external_domains (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY search_terms_key ( date, domains_id, page_id, search_term_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (search_term_id) REFERENCES search_terms (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinc_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_incoming_key (date, domains_id, tvinc_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinc_id) REFERENCES tvectors_inc (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE pageviews_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY pageviews_key ( date, domains_id, page_id, ip_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (ip_id) REFERENCES httpstats_clients.ips (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, size_kb INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY bandwidth_key ( date, domains_id, page_id, size_kb ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinn_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_internal_key (date, tvinn_id, count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinn_id) REFERENCES tvectors_inn (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY referers_key ( date, domains_id, page_id, referer_domain_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (referer_domain_id) REFERENCES external_domains (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY search_terms_key ( date, domains_id, page_id, search_term_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id), FOREIGN KEY (search_term_id) REFERENCES search_terms (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, tvinc_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), KEY date (date), UNIQUE KEY traffic_vector_incoming_key (date, domains_id, tvinc_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinc_id) REFERENCES tvectors_inc (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE pageviews_total ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), UNIQUE KEY pageviews_key ( domains_id, page_id, ip_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_total ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id INT NOT NULL, size_kb INT NOT NULL, UNIQUE KEY id (id), UNIQUE KEY bandwidth_key ( domains_id, page_id, size_kb), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_total ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, tvinn_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), UNIQUE KEY traffic_vector_internal_key (domains_id,tvinn_id, count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinn_id) REFERENCES tvectors_inn (id)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_total ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), UNIQUE KEY referers_key ( domains_id, page_id, referer_domain_id, count ), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_total ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), UNIQUE KEY search_terms_key (domains_id,page_id,search_term_id,count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (page_id) REFERENCES pages_paths_full (id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_total ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, tvinc_id INT NOT NULL, count INT NOT NULL, UNIQUE KEY id (id), UNIQUE KEY traffic_vector_incoming_key(domains_id,tvinc_id,count), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did), FOREIGN KEY (tvinc_id) REFERENCES tvectors_inc(id) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE DATABASE httpstats_jobs CHARACTER SET utf8 COLLATE utf8_bin;
USE httpstats_jobs;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE stats_completed_ranges_hrs    ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE stats_completed_ranges_days   ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE stats_completed_ranges_weeks  ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE stats_completed_ranges_months ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE stats_completed_ranges_years  ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE stats_completed_ranges_totals ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`), FOREIGN KEY (domains_id) REFERENCES cluster.domains (did)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE DATABASE httpstats_api CHARACTER SET utf8 COLLATE utf8_bin;
USE httpstats_api;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE users                   ( id INT(11) NOT NULL,            username VARCHAR(255) NOT NULL, password VARCHAR(255) NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `username` (`username`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE api_user_types          ( id INT NOT NULL AUTO_INCREMENT, name VARCHAR(255) NOT NULL,     UNIQUE KEY `id` (`id`),         UNIQUE KEY `name` (`name`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE api_users               ( id INT NOT NULL AUTO_INCREMENT, username VARCHAR(255) NOT NULL, password BLOB NOT NULL,         user_type_id INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `user_type_id` (`user_type_id`), CONSTRAINT `fk_user_type_id` FOREIGN KEY (`user_type_id`) REFERENCES `api_user_types` (`id`), UNIQUE KEY `username` (`username`), UNIQUE KEY `user_and_type` (`username`, `user_type_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE api_actions             ( id INT NOT NULL AUTO_INCREMENT, name VARCHAR(255) NOT NULL,     UNIQUE KEY `id` (`id`),         UNIQUE KEY `name` (`name`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE api_rights_by_user_type ( id INT NOT NULL AUTO_INCREMENT, action_id INT NOT NULL,         user_type_id INT NOT NULL,      allowed TINYINT(1) NOT NULL, UNIQUE KEY `id` (`id`), KEY `user_type_id` (`user_type_id`), CONSTRAINT `fk_usertype_id` FOREIGN KEY (`user_type_id`) REFERENCES `api_user_types` (`id`), KEY `action_id` (`action_id`), CONSTRAINT `fk_action_id` FOREIGN KEY (`action_id`) REFERENCES `api_actions` (`id`), UNIQUE KEY `action_usertype` (`action_id`,`user_type_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
