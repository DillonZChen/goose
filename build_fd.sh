# ensure this script is run after activating your virtual environment

### Build planners
echo
echo "========= Building downward_cpu ========="
echo
cd planners/downward_cpu
python3 build.py
cd ../..
