ZIP_FILE_NAME=zipped_aaai24_logs.tar.gz

ssh cluster1 "cd goose-aaai24/learner;tar -czvf $ZIP_FILE_NAME aaai24_logs/"
scp cluster1:~/goose-aaai24/learner/$ZIP_FILE_NAME .
ssh cluster1 "cd goose-aaai24/learner;rm $ZIP_FILE_NAME"
tar -xzvf $ZIP_FILE_NAME
rm $ZIP_FILE_NAME
