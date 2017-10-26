DROP DATABASE IF EXISTS httpstats_pages;
CREATE DATABASE httpstats_pages;
USE httpstats_pages;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';

CREATE TABLE pages_paths_full     ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL UNIQUE                           )                                                                 ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvectors_inn         ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, a_id INT NOT NULL, b_id INT NOT NULL,                                 UNIQUE KEY `tvector_inn` (`a_id`, `b_id`))   ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE external_domains     ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255) NOT NULL,                                           UNIQUE KEY `name` (`name`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms         ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255) NOT NULL, search_engine_id INT NOT NULL,            UNIQUE KEY `name_sid` (`name`,`search_engine_id`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvectors_inc         ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, referer_domain_id INT NOT NULL, referer_page_id INT NOT NULL, page_id INT NOT NULL,                     UNIQUE KEY `tvector_inc` (`referer_domain_id`,`referer_page_id`,`page_id`) )   ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16; 

CREATE TABLE pageviews_hourly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL, ip_id             INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `pageviews_key` ( `date`, `domains_id`, `page_id`, `ip_id`, `count` ))                    ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_hourly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL,                                 size_bytes INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `bandwidth_key`    ( `date`, `domains_id`, `page_id`, `size_bytes` ))                     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_hourly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinn_id        INT NOT NULL,                                 count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_internal_key` (`date`, `tvinn_id`, `count`))                              ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_hourly      ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `referers_key` ( `date`, `domains_id`, `page_id`, `referer_domain_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_hourly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, search_term_id    INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `search_terms_key` ( `date`, `domains_id`, `page_id`, `search_term_id`, `count` ))     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_hourly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinc_id        INT NOT NULL,                                                          UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_incoming_key` (`date`, `domains_id`, `tvinc_id` ) )                       ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pageviews_daily     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL, ip_id             INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `pageviews_key` ( `date`, `domains_id`, `page_id`, `ip_id`, `count` ))                    ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_daily     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL,                                 size_bytes INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `bandwidth_key`    ( `date`, `domains_id`, `page_id`, `size_bytes` ))                     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_daily         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinn_id        INT NOT NULL,                                 count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_internal_key` (`date`, `tvinn_id`, `count`))                              ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_daily      ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `referers_key` ( `date`, `domains_id`, `page_id`, `referer_domain_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_daily  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, search_term_id    INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `search_terms_key` ( `date`, `domains_id`, `page_id`, `search_term_id`, `count` ))     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_daily         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinc_id        INT NOT NULL,                                                          UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_incoming_key` (`date`, `domains_id`, `tvinc_id` ) )                       ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pageviews_weekly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL, ip_id             INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `pageviews_key` ( `date`, `domains_id`, `page_id`, `ip_id`, `count` ))                    ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_weekly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL,                                 size_bytes INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `bandwidth_key`    ( `date`, `domains_id`, `page_id`, `size_bytes` ))                     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_weekly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinn_id        INT NOT NULL,                                 count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_internal_key` (`date`, `tvinn_id`, `count`))                              ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_weekly      ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `referers_key` ( `date`, `domains_id`, `page_id`, `referer_domain_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_weekly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, search_term_id    INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `search_terms_key` ( `date`, `domains_id`, `page_id`, `search_term_id`, `count` ))     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_weekly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinc_id        INT NOT NULL,                                                          UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_incoming_key` (`date`, `domains_id`, `tvinc_id` ) )                       ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pageviews_monthly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL, ip_id             INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `pageviews_key` ( `date`, `domains_id`, `page_id`, `ip_id`, `count` ))                    ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_monthly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL,                                 size_bytes INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `bandwidth_key`    ( `date`, `domains_id`, `page_id`, `size_bytes` ))                     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_monthly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinn_id        INT NOT NULL,                                 count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_internal_key` (`date`, `tvinn_id`, `count`))                              ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_monthly      ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `referers_key` ( `date`, `domains_id`, `page_id`, `referer_domain_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_monthly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, search_term_id    INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `search_terms_key` ( `date`, `domains_id`, `page_id`, `search_term_id`, `count` ))     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_monthly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinc_id        INT NOT NULL,                                                          UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_incoming_key` (`date`, `domains_id`, `tvinc_id` ) )                       ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pageviews_yearly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL, ip_id             INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `pageviews_key` ( `date`, `domains_id`, `page_id`, `ip_id`, `count` ))                    ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_yearly     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id         INT NOT NULL,                                 size_bytes INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `bandwidth_key`    ( `date`, `domains_id`, `page_id`, `size_bytes` ))                     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_yearly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinn_id        INT NOT NULL,                                 count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_internal_key` (`date`, `tvinn_id`, `count`))                              ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_yearly      ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `referers_key` ( `date`, `domains_id`, `page_id`, `referer_domain_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_yearly  ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, page_id INT NOT NULL, search_term_id    INT NOT NULL, count      INT NOT NULL, UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `search_terms_key` ( `date`, `domains_id`, `page_id`, `search_term_id`, `count` ))     ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_yearly         ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id  INT NOT NULL, tvinc_id        INT NOT NULL,                                                          UNIQUE KEY `id` (`id`), KEY `date` (`date`), UNIQUE KEY `traffic_vector_incoming_key` (`date`, `domains_id`, `tvinc_id` ) )                       ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pageviews_total      ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id    INT NOT NULL, ip_id          INT NOT NULL, count             INT NOT NULL,                            UNIQUE KEY `id` (`id`), UNIQUE KEY `pageviews_key` ( `domains_id`, `page_id`, `ip_id`, `count` ) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE bandwidth_total      ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id    INT NOT NULL, size_bytes     INT NOT NULL,                                                            UNIQUE KEY `id` (`id`), UNIQUE KEY `bandwidth_key` ( `domains_id`, `page_id`, `size_bytes`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinn_total          ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id    INT NOT NULL, count          INT NOT NULL,                                                            UNIQUE KEY `id` (`id`), UNIQUE KEY `traffic_vector_internal_key` (`domains_id`,`page_id`,`count`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE referers_total       ( id INT NOT NULL AUTO_INCREMENT, domains_id  INT NOT NULL, page_id INT NOT NULL, referer_domain_id INT NOT NULL, count      INT NOT NULL,                          UNIQUE KEY `id` (`id`), UNIQUE KEY `referers_key` ( `domains_id`, `page_id`, `referer_domain_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE search_terms_total   ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, page_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL,                                        UNIQUE KEY `id` (`id`), UNIQUE KEY `search_terms_key` (`domains_id`,`page_id`,`search_term_id`,`count`) ) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE tvinc_total          ( id INT NOT NULL AUTO_INCREMENT, domains_id INT NOT NULL, tvinc_id   INT NOT NULL, count          INT NOT NULL,                                                            UNIQUE KEY `id` (`id`), UNIQUE KEY `traffic_vector_incoming_key`(`domains_id`,`tvinc_id`,`count`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
