#
# compilation lines for test parameters

## STAT Tests

SET(MY_VO "testers.eu-emi.eu")

SET(srm_prefix_dpm "srm://cvitbdpm1.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(srm_valid_dpm_stat "${srm_prefix_dpm}/testread0011")
SET(srm_valid_dpm_chmod "${srm_prefix_dpm}/test_change_right")
SET(srm_valid_dir_root "${srm_prefix_dpm}")
SET(srm_valid_dpm_src_file "${srm_valid_dpm_stat}")

SET(srm_prefix_dcache "srm://cork.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/data/${MY_VO}/testgfal2")
SET(srm_valid_dcache_stat "${srm_prefix_dcache}/testread0011")
SET(srm_valid_dcache_chmod "${srm_prefix_dcache}/test_change_right")
SET(srm_valid_dcache_dir_root "${srm_prefix_dcache}")
SET(srm_valid_dcache_src_file "${srm_valid_dcache_stat}")

SET(srm_prefix_storm "srm://emitestbed03.cnaf.infn.it:8444/srm/managerv2?SFN=/${MY_VO}/gfal2tests")
SET(srm_valid_storm_stat "${srm_prefix_storm}/testread0011")
SET(srm_valid_storm_chmod "${srm_prefix_storm}/test_change_right")
SET(srm_valid_storm_dir_root "${srm_prefix_storm}")
SET(srm_valid_storm_src_file "${srm_prefix_storm}")

#SET(srm_prefix_EOS "srm://srm.pic.es/pnfs/pic.es/data/dteam")
#SET(srm_valid_EOS_stat "${srm_prefix_castor}/testread0011")

SET(gsiftp_prefix_dpm "gsiftp://cvitbdpm1.cern.ch/dpm/cern.ch/home/${MY_VO}/gfal2-tests")
SET(gsiftp_valid_dpm_stat "${gsiftp_prefix_dpm}/testread0011")
SET(gsiftp_valid_dpm_src_file "${gsiftp_valid_dpm_stat}")
SET(gsiftp_valid_dpm_chmod "${gsiftp_prefix_dpm}/test_change_right_gsiftp")
SET(gsiftp_valid_dir_root "${gsiftp_prefix_dpm}")


SET(lfc_prefix "lfn:/grid/${MY_VO}")
SET(lfc_stat_ok "${lfc_prefix}/testread0011")
SET(lfc_chmod_ok "${lfc_prefix}/test_change_right")
SET(lfc_valid_dir_root "${lfc_prefix}")


SET(dcap_prefix "gsidcap://cork.desy.de:22128/pnfs/desy.de/data/${MY_VO}/testgfal2")
SET(dcap_stat_ok "${dcap_prefix}/testread0011")
SET(dcap_chmod_ok "${dcap_prefix}/test_change_right")
SET(dcap_valid_dir_root "${dcap_prefix}")

SET(file_base_path "/tmp/")
SET(file_prefix "file://${file_base_path}")
FILE(WRITE "${file_base_path}/testread_0011" "hello world agdlkmgfmklmklklmvc;!:c;:!;:!xc;!:vx!;:bvx!;:!;:o=)=)à=àdg:;;:!:!;!:;b")
FILE(WRITE "${file_base_path}/test_change_right" "testblabl")
SET(file_stat_ok "${file_prefix}/testread_0011")
SET(file_valid_chmod "${file_prefix}/test_change_right")

IF(PLUGIN_FILE)
    stat_test_all("FILE" ${file_stat_ok})
    chmod_test_all("FILE" ${file_valid_chmod} 0565 060 360 767)
    mkdir_test_all("FILE" ${file_prefix})
    rmdir_test_all("FILE" ${file_prefix} ${file_stat_ok})
    test_readdir_full("FILE" ${file_prefix})
    rwt_test_all("FILE" ${file_prefix} 4578)
    rwt_test_all("FILE" ${file_prefix} 1)
    rwt_test_all("FILE" ${file_prefix} 100000)
    rwt_test_seq("FILE" ${file_prefix} 100 4560)
ENDIF(PLUGIN_FILE)

IF(PLUGIN_SRM)
	stat_test_all( "SRM_DPM" ${srm_valid_dpm_stat})
        stat_test_all( "SRM_DCACHE" ${srm_valid_dcache_stat})
        checksum_test_all("SRM_DPM" ${srm_valid_dir_root})
        checksum_test_all("SRM_DCACHE" ${srm_valid_dcache_dir_root})
        checksum_test_simple("SRM_DPM_ADLER32" ${srm_valid_dpm_stat} ADLER32)
        checksum_test_simple("SRM_DPM_MD5" ${srm_valid_dpm_stat} MD5)
        checksum_test_simple("SRM_DCACHE_ADLER32" ${srm_valid_dcache_stat} ADLER32)
       # checksum_test_simple("SRM_DCACHE_MD5" ${srm_valid_dcache_stat} MD5) Dcache does not support dynamic checksum calculation
	mkdir_test_all("SRM_DPM" ${srm_prefix_dpm})
	mkdir_test_all("SRM_DCACHE" ${srm_prefix_dcache})
	chmod_test_all("SRM_DPM" ${srm_valid_dpm_chmod} 0565 060 360 767)	
	rmdir_test_all("SRM_DPM" ${srm_valid_dir_root} ${srm_valid_dpm_stat})	
	test_readdir_full("SRM_DPM" ${srm_valid_dir_root} )	
	rwt_test_all("SRM_DPM" ${srm_valid_dir_root} 4578)
	rwt_test_all("SRM_DPM_little" ${srm_valid_dir_root} 10)
	rwt_test_all("SRM_DPM_single" ${srm_valid_dir_root} 1)	
	rwt_test_all("SRM_DCAP" ${srm_valid_dcache_dir_root} 4578)
	rwt_test_all("SRM_DCAP_little" ${srm_valid_dcache_dir_root} 10)	
	rwt_test_all("SRM_DCAP_single" ${srm_valid_dcache_dir_root} 1)		
	rwt_test_seq("SRM_DPM" ${srm_valid_dir_root} 100 4560)	
	rwt_test_seq("SRM_DPM_unit" ${srm_valid_dir_root} 1 10)			
	rwt_test_seq("SRM_DCAP" ${srm_valid_dir_root} 100 4560)	

        rwt_test_seq("SRM_STORM" ${srm_prefix_storm} 100 4560)
		
#	chmod_test_all("SRM_DCACHE" ${srm_valid_dcache_chmod} 0565 060 360 767)	 -> disabled, since unavailable on dcache
#	stat_test_all( "SRM_EOS" ${srm_valid_EOS_stat})
ENDIF(PLUGIN_SRM)

IF(PLUGIN_LFC)
	stat_test_all( "LFC" ${lfc_stat_ok})
	mkdir_test_all("LFC" ${lfc_prefix})	
	chmod_test_all("LFC" ${lfc_chmod_ok} 0565 060 0360 0767)	
	rmdir_test_all("LFC" ${lfc_valid_dir_root} ${lfc_stat_ok})		
	test_readdir_full("LFC" ${lfc_valid_dir_root} )
ENDIF(PLUGIN_LFC)


IF(PLUGIN_DCAP)
	stat_test_all( "DCAP" ${dcap_stat_ok})
	mkdir_test_all("DCAP" ${dcap_prefix})	
	chmod_test_all("DCAP" ${dcap_chmod_ok} 0565 000 0320 0767)	
	chmod_test_all("DCAP_2" ${dcap_chmod_ok} 000 0555 0666 0777)
	#test_readdir_full("DCAP" ${dcap_valid_dir_root} )	
	#rmdir_test_all("DCAP" ${dcap_valid_dir_root} ${dcap_stat_ok})	
	rwt_test_all("DCAP" ${dcap_valid_dir_root} 4578)	
	rwt_test_seq("DCAP" ${dcap_valid_dir_root} 100 4560)	
ENDIF(PLUGIN_DCAP)


IF(PLUGIN_GRIDFTP)
	stat_test_all( "GRIDFTP" ${gsiftp_valid_dpm_stat})
        checksum_test_all("GRIDFTP" ${gsiftp_valid_dir_root})
        checksum_test_simple("GRIDFTP_ADLER32" ${gsiftp_valid_dpm_stat} ADLER32)
        checksum_test_simple("GRIDFTP_MD5" ${gsiftp_valid_dpm_stat} MD5)
        checksum_test_simple("GRIDFTP_CRC32" ${gsiftp_valid_dpm_stat} CRC32)
	mkdir_test_all("GRIDFTP" ${gsiftp_prefix_dpm})	
	chmod_test_all("GRIDFTP" ${gsiftp_valid_dpm_chmod} 0565 060 0360 0767)	
	chmod_test_all("GRIDFTP_2" ${gsiftp_valid_dpm_chmod} 000 0555 0666 0777)	
	rmdir_test_all("GRIDFTP" ${gsiftp_valid_dir_root} ${gsiftp_valid_dpm_stat})	
	test_readdir_full("GRIDFTP" ${gsiftp_valid_dir_root} )	
	rwt_test_all("GRIDFTP" ${gsiftp_valid_dir_root} 4578)
	rwt_test_all("GRIDFTP_single" ${gsiftp_valid_dir_root} 1)		
	rwt_test_seq("GRIDFTP" ${gsiftp_valid_dir_root} 100 4560)
	rwt_test_seq("GRIDFTP_unit" ${gsiftp_valid_dir_root} 1 10)		
ENDIF(PLUGIN_GRIDFTP)

IF (MAIN_TRANSFER)
	copy_file_test_full("GRIDFTP_DPM"  ${gsiftp_valid_dpm_src_file} ${gsiftp_prefix_dpm})	
	copy_file_test_full("SRM_DPM"  ${srm_valid_dpm_src_file} ${srm_valid_dir_root})
        copy_file_test_full("SRM_DPM_TO_DCACHE"  ${srm_valid_dpm_src_file} ${srm_valid_dcache_dir_root})
        copy_file_test_full("SRM_DCACHE_TO_SRM"  ${srm_valid_dcache_src_file} ${srm_valid_dir_root})
        copy_file_test_full("GSIFTP_DPM_TO_SRM_DCACHE"  ${gsiftp_valid_dpm_src_file} ${srm_valid_dcache_dir_root})
	copy_file_test_full("SRM_DCACHE"  ${srm_valid_dcache_src_file} ${srm_valid_dcache_dir_root})
	copy_file_test_full("SRM_TO_GRIDFTP"  ${srm_valid_dpm_src_file} ${gsiftp_prefix_dpm})
        copy_file_test_full("GRIDFTP_TO_SRM"  ${gsiftp_valid_dpm_src_file} ${srm_valid_dir_root})

        # storm <-> storm
        copy_file_test_full("STORM_TO_STORM" ${srm_valid_storm_stat}  ${srm_prefix_storm})
        # storm -> dpm
        copy_file_test_full("STORM_TO_SRM_DPM" ${srm_valid_storm_stat}  ${srm_valid_dir_root})
        # storm -> dcache
        copy_file_test_full("STORM_TO_SRM_DCACHE" ${srm_valid_storm_stat}  ${srm_valid_dcache_dir_root})

        # local transfer
        #

        # local <-> DPM
        copy_file_test_simple("FILE_TO_SRM_DPM" ${file_stat_ok} ${srm_valid_dir_root})
        copy_file_test_simple("SRM_DPM_TO_FILE" ${srm_valid_dpm_src_file}  ${file_prefix})

        # local <-> dcache
        copy_file_test_simple("FILE_TO_SRM_DCACHE" ${file_stat_ok} ${srm_valid_dcache_dir_root})
        copy_file_test_simple("SRM_DCACHE_TO_FILE" ${srm_valid_dcache_src_file}  ${file_prefix})
        copy_file_test_simple("FILE_TO_FILE" ${file_stat_ok}  ${file_prefix})

        # gsiftp dpm <-> local
        copy_file_test_simple("GSIFTP_TO_FILE" ${gsiftp_valid_dpm_src_file}  ${file_prefix})
        copy_file_test_simple("FILE_TO_GSIFTP" ${file_stat_ok}  ${gsiftp_prefix_dpm})

        # local <-> storm
        copy_file_test_simple("STORM_TO_FILE" ${srm_valid_storm_stat}  ${file_prefix})
        copy_file_test_simple("FILE_TO_STORM" ${file_stat_ok}  ${srm_prefix_storm})


ENDIF (MAIN_TRANSFER)
