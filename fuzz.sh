#!/usr/bin/zsh

CRASHING=0
RUNS=0
SEED_FILE=samples/conditional.mx

# We clean the workspace
rm -rf fuzz
mkdir -p fuzz

while true;do
    id=$(head -c 8 /dev/urandom | base32 | tr -d "=")
    TEST_FILE=fuzz/case_$id.mx
    cp $SEED_FILE $TEST_FILE
    cat $TEST_FILE | radamsa > $TEST_FILE
    timeout -s INT 1 ./min-vm --run $TEST_FILE > /dev/null 2>&1
    
    if [ $? = "139" ]
    then
        CRASHING=$((CRASHING + 1))
    else
        rm $TEST_FILE
    fi

    RUNS=$((RUNS + 1))
    echo -n "\rRuns: $RUNS Crashes: $CRASHING"
done
