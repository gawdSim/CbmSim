#!/usr/bin/bash

# globals
root_dir="/home/seang/Dev/Git/CbmSim/"
data_in_dir="${root_dir}data/inputs/"
data_out_dir="${root_dir}data/outputs/"
build_dir="${root_dir}build/"
debug_dir="${build_dir}debug/"
scripts_dir="${root_dir}scripts/"

build_file="build_file_template_tune_09262022.bld"
sess_file="TESTS.sess"
binary="${build_dir}cbm_sim"

passed_tests=0
num_tests=50

printf "Running all tests...\n"
# workflow 0 test cases: building a simulation

## valid test cases

curr_date=$(date +%m%d%Y)
if ! $binary -b $build_file -o TEST_CASE_1
then
	printf "TEST CASE 1 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_1" ]
	then
		printf "TEST CASE 1 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_1' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_1/TEST_CASE_1_$curr_date.sim" ]
	then
		printf "TEST CASE 1 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_1_$curr_date.sim' was not produced\n"
	else
		printf "TEST CASE 1 PASSED\n"
		(( passed_tests++ ))
	fi
fi
 
curr_date=$(date +%m%d%Y)
if ! $binary -b $build_file -o TEST_CASE_2 -r PC,GO -p PC,GO -w PFPC,MFNC
then
	printf "TEST CASE 2 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_2" ]
	then
		printf "TEST CASE 2 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_2' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_2/TEST_CASE_2_$curr_date.sim" ]
	then
		printf "TEST CASE 2 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_2_$curr_date.sim' was not produced\n"
	else
		printf "TEST CASE 2 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $binary -b $build_file -o TEST_CASE_3 -v TUI -r PC,GO -p PC,GO -w PFPC,MFNC
then
	printf "TEST CASE 1 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_3" ]
	then
		printf "TEST CASE 3 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_3' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_3/TEST_CASE_3_$curr_date.sim" ]
	then
		printf "TEST CASE 3 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_3_$curr_date.sim' was not produced\n"
	else
		printf "TEST CASE 3 PASSED\n"
		(( passed_tests++ ))
	fi
fi

## invalid test cases

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -o TEST_CASE_4 -v GUI -r PC,GO -p PC,GO -w PFPC,MFNC > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify visual mode 'GUI' in build mode. Exiting..." ]]
then
	printf "TEST CASE 4 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 4 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -o TEST_CASE_5 -s $sess_file -r PC,GO -p PC,GO -w PFPC,MFNC > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify both session and build file. Exiting..." ]]
then
	printf "TEST CASE 5 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 5 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -s $sess_file -r PC,GO -p PC,GO -w PFPC,MFNC > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify both session and build file. Exiting..." ]]
then
	printf "TEST CASE 6 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 6 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -r PC,GO -p PC,GO -w PFPC,MFNC > /dev/null; } 2>&1 )"
if [[ $err =~ "You must specify an output basename. Exiting..." ]]
then
	printf "TEST CASE 7 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 7 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -v TUI > /dev/null; } 2>&1 )"
if [[ $err =~ "You must specify an output basename. Exiting..." ]]
then
	printf "TEST CASE 8 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 8 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -v GUI > /dev/null; } 2>&1 )"
if [[ $err =~ "You must specify an output basename. Exiting..." ]]
then
	printf "TEST CASE 9 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 9 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -b $build_file -i TEST_INPUT.sim > /dev/null; } 2>&1 )"
if [[ $err =~ "You must specify an output basename. Exiting..." ]]
then
	printf "TEST CASE 10 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 10 FAILED\n"
fi

# workflow 1 test cases: building a simulation

workflow_1_basename="WORKFLOW_1_INPUT"
printf "Generating simulation for workflow 1 test cases...\n"
curr_date=$(date +%m%d%Y)
$binary -b $build_file -o $workflow_1_basename
workflow_1_input="${workflow_1_basename}_${curr_date}.sim"

## valid test cases
curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_11 > /dev/null 2>&1 )
then
	printf "TEST CASE 11 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_11" ]
	then
		printf "TEST CASE 11 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_11' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_11/TEST_CASE_11_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_11/TEST_CASE_11_$curr_date.txt" ] 
	then
		printf "TEST CASE 11 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_11_$curr_date.sim' and info file 'TEST_CASE_11_$curr_date.txt' were not produced\n"
	else
		printf "TEST CASE 11 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_12 -r MF > /dev/null 2>&1 )
then
	printf "TEST CASE 12 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_12" ]
	then
		printf "TEST CASE 12 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_12' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_12/TEST_CASE_12_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_12/TEST_CASE_12_$curr_date.txt" ] 
	then
		printf "TEST CASE 12 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_12_$curr_date.sim' and info file 'TEST_CASE_12_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_12/TEST_CASE_12_MF_RASTER_$curr_date.bin" ]
	then
		printf "TEST CASE 12 FAILED\n"
		printf "\tREASON: Raster file 'TEST_CASE_12_MF_RASTER_$curr_date.bin' was not produced\n"
	else
		printf "TEST CASE 12 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_13 -r MF,GO > /dev/null 2>&1 )
then
	printf "TEST CASE 13 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_13" ]
	then
		printf "TEST CASE 13 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_13' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_13/TEST_CASE_13_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_13/TEST_CASE_13_$curr_date.txt" ] 
	then
		printf "TEST CASE 13 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_13_$curr_date.sim' and info file 'TEST_CASE_13_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_13/TEST_CASE_13_MF_RASTER_$curr_date.bin" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_13/TEST_CASE_13_GO_RASTER_$curr_date.bin" ]
	then
		printf "TEST CASE 13 FAILED\n"
		printf "\tREASON: Raster files 'TEST_CASE_13_MF_RASTER_$curr_date.bin' and 'TEST_CASE_13_GO_RASTER_$curr_date.bin' were not produced\n"
	else
		printf "TEST CASE 13 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_14 -r GO,MF > /dev/null 2>&1 )
then
	printf "TEST CASE 14 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_14" ]
	then
		printf "TEST CASE 14 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_14' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_14/TEST_CASE_14_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_14/TEST_CASE_14_$curr_date.txt" ] 
	then
		printf "TEST CASE 14 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_14_$curr_date.sim' and info file 'TEST_CASE_14_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_14/TEST_CASE_14_MF_RASTER_$curr_date.bin" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_14/TEST_CASE_14_GO_RASTER_$curr_date.bin" ]
	then
		printf "TEST CASE 14 FAILED\n"
		printf "\tREASON: Raster files 'TEST_CASE_14_MF_RASTER_$curr_date.bin' and 'TEST_CASE_14_GO_RASTER_$curr_date.bin' were not produced\n"
	else
		printf "TEST CASE 14 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_15 -p MF > /dev/null 2>&1 )
then
	printf "TEST CASE 15 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_15" ]
	then
		printf "TEST CASE 15 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_15' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_15/TEST_CASE_15_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_15/TEST_CASE_15_$curr_date.txt" ] 
	then
		printf "TEST CASE 15 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_15_$curr_date.sim' and info file 'TEST_CASE_15_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_15/TEST_CASE_15_MF_PSTH_$curr_date.bin" ]
	then
		printf "TEST CASE 15 FAILED\n"
		printf "\tREASON: PSTH file 'TEST_CASE_15_MF_PSTH_$curr_date.bin' was not produced\n"
	else
		printf "TEST CASE 15 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_16 -p MF,GO > /dev/null 2>&1 )
then
	printf "TEST CASE 16 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_16" ]
	then
		printf "TEST CASE 16 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_16' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_16/TEST_CASE_16_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_16/TEST_CASE_16_$curr_date.txt" ] 
	then
		printf "TEST CASE 16 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_16_$curr_date.sim' and info file 'TEST_CASE_16_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_16/TEST_CASE_16_MF_PSTH_$curr_date.bin" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_16/TEST_CASE_16_GO_PSTH_$curr_date.bin" ]
	then
		printf "TEST CASE 16 FAILED\n"
		printf "\tREASON: PSTH files 'TEST_CASE_16_MF_PSTH_$curr_date.bin' and 'TEST_CASE_16_GO_PSTH_$curr_date.bin' were not produced\n"
	else
		printf "TEST CASE 16 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_17 -p GO,MF > /dev/null 2>&1 )
then
	printf "TEST CASE 17 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_17" ]
	then
		printf "TEST CASE 17 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_17' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_17/TEST_CASE_17_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_17/TEST_CASE_17_$curr_date.txt" ] 
	then
		printf "TEST CASE 17 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_17_$curr_date.sim' and info file 'TEST_CASE_17_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_17/TEST_CASE_17_MF_PSTH_$curr_date.bin" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_17/TEST_CASE_17_GO_PSTH_$curr_date.bin" ]
	then
		printf "TEST CASE 17 FAILED\n"
		printf "\tREASON: PSTH files 'TEST_CASE_17_MF_PSTH_$curr_date.bin' and 'TEST_CASE_17_GO_PSTH_$curr_date.bin' were not produced\n"
	else
		printf "TEST CASE 17 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_18 -w PFPC > /dev/null 2>&1 )
then
	printf "TEST CASE 18 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_18" ]
	then
		printf "TEST CASE 18 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_18' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_18/TEST_CASE_18_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_18/TEST_CASE_18_$curr_date.txt" ] 
	then
		printf "TEST CASE 18 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_18_$curr_date.sim' and info file 'TEST_CASE_18_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_18/TEST_CASE_18_PFPC_WEIGHTS_${curr_date}_TRIAL_0.bin" ]
	then
		printf "TEST CASE 18 FAILED\n"
		printf "\tREASON: Weights file 'TEST_CASE_18_PFPC_WEIGHTS_${curr_date}_TRIAL_0.bin' was not produced\n"
	else
		printf "TEST CASE 18 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_19 -w PFPC,MFNC > /dev/null 2>&1 )
then
	printf "TEST CASE 19 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_19" ]
	then
		printf "TEST CASE 19 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_19' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_19/TEST_CASE_19_$curr_date.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_19/TEST_CASE_19_$curr_date.txt" ] 
	then
		printf "TEST CASE 19 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_19_$curr_date.sim' and info file 'TEST_CASE_19_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_19/TEST_CASE_19_PFPC_WEIGHTS_${curr_date}_TRIAL_0.bin" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_19/TEST_CASE_19_MFNC_WEIGHTS_${curr_date}_TRIAL_0.bin" ]
	then
		printf "TEST CASE 19 FAILED\n"
		printf "\tREASON: Weights files 'TEST_CASE_19_PFPC_WEIGHTS_${curr_date}_TRIAL_0.bin' and 'TEST_CASE_19_MFNC_WEIGHTS_${curr_date}_TRIAL_0.bin' were not produced\n"
	else
		printf "TEST CASE 19 PASSED\n"
		(( passed_tests++ ))
	fi
fi

curr_date=$(date +%m%d%Y)
if ! $( $binary -i $workflow_1_input -s $sess_file -o TEST_CASE_20 -w MFNC,PFPC > /dev/null 2>&1 )
then
	printf "TEST CASE 20 FAILED\n"
	printf "\tREASON: the command returned non-zero exit status\n"
else
	if ! [ -d "${data_out_dir}TEST_CASE_20" ]
	then
		printf "TEST CASE 20 FAILED\n"
		printf "\tREASON: Output folder 'TEST_CASE_20' was not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_20/TEST_CASE_20_${curr_date}.sim" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_20/TEST_CASE_20_${curr_date}.txt" ] 
	then
		printf "TEST CASE 20 FAILED\n"
		printf "\tREASON: Output simulation 'TEST_CASE_20_$curr_date.sim' and info file 'TEST_CASE_20_$curr_date.txt' were not produced\n"
	elif ! [ -e "${data_out_dir}TEST_CASE_20/TEST_CASE_20_PFPC_WEIGHTS_${curr_date}_TRIAL_0.bin" ] && \
		 ! [ -e "${data_out_dir}TEST_CASE_20/TEST_CASE_20_MFNC_WEIGHTS_${curr_date}_TRIAL_0.bin" ]
	then
		printf "TEST CASE 20 FAILED\n"
		printf "\tREASON: Weights files 'TEST_CASE_20_MFNC_WEIGHTS_${curr_date}_TRIAL_0.bin' and 'TEST_CASE_20_PFPC_WEIGHTS_${curr_date}_TRIAL_0.bin' were not produced\n"
	else
		printf "TEST CASE 20 PASSED\n"
		(( passed_tests++ ))
	fi
fi

## invalid test cases

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -b $build_file -o TEST_CASE_21 -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify both session and build file. Exiting..." ]]
then
	printf "TEST CASE 21 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 21 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -b $build_file -i $workflow_1_input -o TEST_CASE_22 -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify both session and build file. Exiting..." ]]
then
	printf "TEST CASE 22 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 22 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -b $build_file -i $workflow_1_input -o TEST_CASE_23 -r MF,GO -p MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify both session and build file. Exiting..." ]]
then
	printf "TEST CASE 23 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 23 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -b $build_file -i $workflow_1_input -o TEST_CASE_24 -r MF,GO -p MF,GO -w PFPC,MFNC > /dev/null; } 2>&1 )"
if [[ $err =~ "Cannot specify both session and build file. Exiting..." ]]
then
	printf "TEST CASE 24 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 24 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "no input simulation specified in run mode. exiting..." ]]
then
	printf "TEST CASE 25 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 25 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -o TEST_CASE_26 -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "no input simulation specified in run mode. exiting..." ]]
then
	printf "TEST CASE 26 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 26 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i NONEXISTENT_FILE.sim -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "Could not find input simulation file 'NONEXISTENT_FILE.sim'. Exiting..." ]]
then
	printf "TEST CASE 27 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 27 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i NONEXISTENT_FILE.sim -o TEST_CASE_28 -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "Could not find input simulation file 'NONEXISTENT_FILE.sim'. Exiting..." ]]
then
	printf "TEST CASE 28 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 28 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -r MF,GO > /dev/null; } 2>&1 )"
if [[ $err =~ "You must specify an output basename. Exiting..." ]]
then
	printf "TEST CASE 29 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 29 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_30 -r > /dev/null; } 2>&1 )"
if [[ $err =~ "No parameter given for option '-r'. Exiting..." ]]
then
	printf "TEST CASE 30 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 30 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_31 -p > /dev/null; } 2>&1 )"
if [[ $err =~ "No parameter given for option '-p'. Exiting..." ]]
then
	printf "TEST CASE 31 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 31 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_32 -w > /dev/null; } 2>&1 )"
if [[ $err =~ "No parameter given for option '-w'. Exiting..." ]]
then
	printf "TEST CASE 32 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 32 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_33 -r , > /dev/null; } 2>&1 )"
if [[ $err =~ "Invalid placement of comma for option '-r'. Exiting..." ]]
then
	printf "TEST CASE 33 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 33 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_34 -p , > /dev/null; } 2>&1 )"
if [[ $err =~ "Invalid placement of comma for option '-p'. Exiting..." ]]
then
	printf "TEST CASE 34 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 34 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_35 -w , > /dev/null; } 2>&1 )"
if [[ $err =~ "Invalid placement of comma for option '-w'. Exiting..." ]]
then
	printf "TEST CASE 35 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 35 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_36 -r MF,GO,ER > /dev/null; } 2>&1 )"
if [[ $err =~ "Invalid cell id 'ER' found for option '-r'. Exiting..." ]]
then
	printf "TEST CASE 36 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 36 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_37 -p MF,GO,ER > /dev/null; } 2>&1 )"
if [[ $err =~ "Invalid cell id 'ER' found for option '-p'. Exiting..." ]]
then
	printf "TEST CASE 37 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 37 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_38 -w PFPC,ERRO > /dev/null; } 2>&1 )"
if [[ $err =~ "Invalid weights id 'ERRO' found for option '-w'. Exiting..." ]]
then
	printf "TEST CASE 38 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 38 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary --pfpc-off -s $sess_file -i $workflow_1_input -o TEST_CASE_39 --binary > /dev/null; } 2>&1 )"
if [[ $err =~ "Mutually exclusive or duplicate pfpc plasticity arguments found. Exiting..." ]]
then
	printf "TEST CASE 39 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 39 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary --binary -s $sess_file -i $workflow_1_input -o TEST_CASE_40 --cascade > /dev/null; } 2>&1 )"
if [[ $err =~ "Mutually exclusive or duplicate pfpc plasticity arguments found. Exiting..." ]]
then
	printf "TEST CASE 40 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 40 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary --cascade -s $sess_file -i $workflow_1_input -o TEST_CASE_41 --binary > /dev/null; } 2>&1 )"
if [[ $err =~ "Mutually exclusive or duplicate pfpc plasticity arguments found. Exiting..." ]]
then
	printf "TEST CASE 41 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 41 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary --binary -s $sess_file -i $workflow_1_input -o TEST_CASE_42 --binary > /dev/null; } 2>&1 )"
if [[ $err =~ "Mutually exclusive or duplicate pfpc plasticity arguments found. Exiting..." ]]
then
	printf "TEST CASE 42 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 42 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary --mfnc-off -s $sess_file -i $workflow_1_input -o TEST_CASE_43 --mfnc-off > /dev/null; } 2>&1 )"
if [[ $err =~ "Duplicate mfnc plasticity arguments found. Exiting..." ]]
then
	printf "TEST CASE 43 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 43 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file --session $sess_file -i $workflow_1_input -o TEST_CASE_44 > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-s'. Exiting..." ]]
then
	printf "TEST CASE 44 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 44 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input --input $workflow_1_input -o TEST_CASE_45 > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-i'. Exiting..." ]]
then
	printf "TEST CASE 45 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 45 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_46 --output TEST_CASE_46 > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-o'. Exiting..." ]]
then
	printf "TEST CASE 46 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 46 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_47 -r PC --raster PC > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-r'. Exiting..." ]]
then
	printf "TEST CASE 47 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 47 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_48 -p PC --psth PC > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-p'. Exiting..." ]]
then
	printf "TEST CASE 48 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 48 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -s $sess_file -i $workflow_1_input -o TEST_CASE_49 -w PFPC --weights PFPC > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-w'. Exiting..." ]]
then
	printf "TEST CASE 49 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 49 FAILED\n"
fi

curr_date="$(date +%m%d%Y)"
err="$( { $binary -v TUI --visual TUI -s $sess_file -i $workflow_1_input -o TEST_CASE_50 > /dev/null; } 2>&1 )"
if [[ $err =~ "Found both short and long form of option '-v'. Exiting..." ]]
then
	printf "TEST CASE 50 PASSED\n"
	(( passed_tests++ ))
else
	printf "TEST CASE 50 FAILED\n"
fi

printf "All tests finished.\n"
printf "${passed_tests}/${num_tests} passed.\n"
rm -rf ${data_out_dir}TEST_CASE_*
rm -rf "${data_out_dir}${workflow_1_basename}/"
