#! /usr/bin/env bash
DATASET=dataset_a # change this to dataset_b if you wish to avoid medical images
SEARCH_ITERATIONS=100 # You can change this variable to increase/decrease the number of times the search is run to make debugging quicker

# Check if the script is being run on linux or macOS
if [ "$(uname)" == "Darwin" ]; then
    IS_MACOS=1 # Darwin means macOS
else
    IS_MACOS=0 # assume running linux e.g. WSL/Ubuntu
fi

# import heaptrack functions
. ./provided_functions/heaptrack.sh

echo -e "\nCompiling stats functions"
echo -e "-------------------------------------"
make test SILENT=1
echo -e "\nTesting stats functions"
echo -e "-------------------------------------"
python3 provided_functions/test_stats.py

echo -e "\nCompiling transformations functions"
echo -e "-------------------------------------"
make test_transformations SILENT=1
echo -e "\nTesting transformations"
echo -e "-------------------------------------"
python3 provided_functions/test_transformations.py

echo -e "\nCompiling database"
echo -e "-------------------------------------"
make database SILENT=1

echo -e "\nCreating database"
echo -e "-------------------------------------"
DATABASE_FILE=../outputs/database.tsv

if [ $IS_MACOS -eq 1 ]; then
    # we use xctrace to measure memory usage on macOS
    codesign --remove-signature -s - -f --entitlements ./provided_functions/macos_only/entitlements.plist ../outputs/database # ensures we can measure memory
    rm -r ../outputs/memory.trace
    xctrace record --output ../outputs/memory.trace --template 'Leaks' --target-stdout $DATABASE_FILE --launch ../outputs/database ../data/$DATASET/known > /dev/null
    echo "Run the command 'open ../outputs/memory.trace' to see your memory usage."
    # ../outputs/database ../data/$DATASET/known > ${DATABASE_FILE} # this is the command to run without memory profiling
else
    # we use heaptrack to measure memory usage on linux (WSL/Ubuntu)
    MEMORY_FILE=../outputs/heaptrack_summary.txt

    check_heaptrack_installed # downloads and installs heaptrack if it doesn't exist

    /usr/local/bin/heaptrack -o ../outputs/heaptrack ../outputs/database ../data/$DATASET/known 2> $MEMORY_FILE | # store heaptrack stats (stderr) in heaptrack_summary 
    tail -n +3 | # Remove first two lines (added by heaptrack)
    head -n -3 > ${DATABASE_FILE} # Remove last 3 lines (added by heaptrack)
    PEAK_HEAP_MEM=$(/usr/local/bin/heaptrack -a ../outputs/heaptrack.gz | grep "peak heap memory")
    echo $PEAK_HEAP_MEM >> $MEMORY_FILE
    python3 provided_functions/check_memory.py ../outputs/heaptrack_summary.txt
fi

echo -e "\nCompiling search"
echo -e "-------------------------------------"
make search SILENT=1
# Note: search output is redirected to database.tsv. stdout of time is ignored, but stderr of time is redirected to stdout out which is stored in TIME variable

# NOTE: We take the average search time over SEARCH_ITERATIONS runs

echo -e "\nSearching without transformations"
echo -e "-------------------------------------"
SEARCH_TIMES_FILE=../outputs/search_times.tsv
echo "time" > $SEARCH_TIMES_FILE
TIMEFORMAT='%4R'
for ((i=1;i<=SEARCH_ITERATIONS;i++)); do
    SEARCH_TIME_NO_TF=$({ time ../outputs/search ../data/$DATASET/unknown_no_transform ../outputs/database.tsv > ../outputs/matchlist_no_tf.tsv ; } 2>&1 >/dev/null)
    echo $SEARCH_TIME_NO_TF >> $SEARCH_TIMES_FILE
done
python3 provided_functions/check_matches.py ../data/$DATASET/unknown_no_transform/answers.tsv ../outputs/matchlist_no_tf.tsv
python3 provided_functions/check_time.py ../outputs/search_times.tsv
echo ""

echo "Searching with transformations"
echo -e "-------------------------------------"
echo "time" > $SEARCH_TIMES_FILE
for ((i=1;i<=SEARCH_ITERATIONS;i++)); do
    SEARCH_TIME_TF=$({ time ../outputs/search ../data/$DATASET/unknown_transformed ../outputs/database.tsv > ../outputs/matchlist_tf.tsv ; } 2>&1 >/dev/null)
    echo $SEARCH_TIME_TF >> $SEARCH_TIMES_FILE
done
python3 provided_functions/check_matches.py ../data/$DATASET/unknown_transformed/answers.tsv ../outputs/matchlist_tf.tsv
python3 provided_functions/check_time.py ../outputs/search_times.tsv
echo ""