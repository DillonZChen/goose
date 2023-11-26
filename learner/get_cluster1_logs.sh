ZIP_FILE_NAME=zipped_aaai24_logs.tar.gz

ssh cluster1 "cd goose/learner;tar -czvf $ZIP_FILE_NAME aaai24_logs/"
scp cluster1:~/goose/learner/$ZIP_FILE_NAME .
tar -xzvf $ZIP_FILE_NAME
rm $ZIP_FILE_NAME
