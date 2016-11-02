SET ORACLE_HOME=C:\DEV\COMP\OracleClient32
SET PATH=.;%PATH%;%ORACLE_HOME%;%ORACLE_HOME%\sdk:.
call road_name_gen.exe -s road.name.ini addr_c road_name.xml tb_addr_map_temp_ref_jibun create
call road_name_gen.exe -s road.name.ini bldg_c road_name.xml tb_addr_map_tmp2 create
call road_name_gen.exe -s road.name.ini rdcode_c road_name.xml tb_addr_map_temp_rd_cd create