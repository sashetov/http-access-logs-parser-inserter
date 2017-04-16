SELECT  hourly.start_ts  AS hst, hourly.end_ts  AS  het,
        daily.start_ts   AS dst, daily.end_ts   AS  det,
        monthly.start_ts AS mst, monthly.end_ts AS  met,
        yearly.start_ts  AS yst, yearly.end_ts  AS  yet 
    FROM statistics_hourly.created_ranges_hourly hourly
        LEFT JOIN statistics_daily.created_ranges_daily daily
            ON hourly.domains_id = daily.domains_id
        LEFT JOIN statistics_monthly.created_ranges_monthly monthly
            ON hourly.domains_id = monthly.domains_id
        LEFT JOIN statistics_yearly.created_ranges_yearly yearly
            ON hourly.domains_id = yearly.domains_id;
