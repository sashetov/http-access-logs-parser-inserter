DROP DATABASE IF EXISTS statistics_weekly;
CREATE DATABASE statistics_weekly;
USE statistics_weekly;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
--SET GLOBAL innodb_compression_algorithm='lzma';
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';

CREATE TABLE hits_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE visits_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_ip_count` ( `date`, `domains_id`, `ip_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pageviews_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, ip_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_page_ip_count` ( `date`, `domains_id`, `page_id`, `ip_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE bandwidth_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, page_id INT NOT NULL, size_bytes INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_page_count` ( `date`, `domains_id`, `page_id`, `size_bytes` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE locations_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, country_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_location_count` ( `date`, `domains_id`, `country_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE oses_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, os_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_os_count` ( `date`, `domains_id`, `os_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE referers_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, referer_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_referer_count` ( `date`, `domains_id`, `referer_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE search_terms_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, referer_id INT NOT NULL, search_term_id INT NOT NULL, count INT NOT NULL, KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_referer_search_count` ( `date`, `domains_id`, `referer_id`, `search_term_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE created_ranges_weekly ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
