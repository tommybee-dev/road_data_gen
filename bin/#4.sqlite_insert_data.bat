SET PATH=.;%PATH%
call road_name_gen.exe -s road.name.ini addr_s road_name.xml tb_addr_map_temp_ref_jibun insert
call road_name_gen.exe -s road.name.ini bldg_s road_name.xml tb_addr_map_tmp2 insert
call road_name_gen.exe -s road.name.ini rdcode_s road_name.xml tb_addr_map_temp_rd_cd insert