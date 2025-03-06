
mkdir tmp_velo
cd tmp_velo

echo
echo ******                    ******
echo ****** Compiling arango-cpp ******
echo ******                    ******
echo

echo git clone arango-cpp...
git clone  --recurse-submodules https://github.com/sdmytrievs/arango-cpp.git
cd arango-cpp

echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -DBUILD_EXAMPLES=OFF  -DBULID_LOCAL_TESTS=OFF -DBULID_REMOTE_TESTS=OFF  -A x64 -S . -B build
echo "Building..."
cmake --build build --target install  --config Release

cd ..

echo
echo ******                    ******
echo ****** Compiling JSONIO ******
echo ******                    ******
echo

echo git clone jsonio...
git clone https://github.com/sdmytrievs/jsonio.git
cd jsonio

echo "Configuring..."
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -DBuildExamples=OFF  -DBuildTests=OFF -A x64 -S . -B build
echo "Building..."
cmake --build build --target install  --config Release

cd ..\..

REM Housekeeping
rd /s /q tmp_velo
