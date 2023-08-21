FILE="goose.sif"
if [ ! -f "$FILE" ]; then
    sh singularity/build_singularity_container.sh
fi

conda deactivate

cd downward
rm -rf builds
sh scripts/build_fd_gpu.sh
cd ..

cd powerlifted
rm -rf builds
sh scripts/build_pwl_gpu.sh
cd ..

