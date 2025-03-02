
if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2019" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

mkdir tmp_velo
cd tmp_velo

echo
echo ******                    ******
echo ****** Compiling jsonarango ******
echo ******                    ******
echo

echo git clone jsonarango...
git clone  --recurse-submodules https://bitbucket.org/gems4/jsonarango.git
cd jsonarango

echo "Configuring..."
cmake -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -DBUILD_EXAMPLES=OFF  -DBULID_LOCAL_TESTS=OFF -DBULID_REMOTE_TESTS=OFF  .. -A x64 -S . -B build
echo "Building..."
cmake --build build --target install

cd ..

echo
echo ******                    ******
echo ****** Compiling JSONIO17 ******
echo ******                    ******
echo

echo git clone jsonio17...
git clone https://bitbucket.org/gems4/jsonio17.git
cd jsonio17

echo "Configuring..."
cmake -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -DBuildExamples=OFF  -DBuildTests=OFF .. -A x64 -S . -B build
echo "Building..."
cmake --build build --target install

cd ..

echo
echo ******                       ******
echo ****** Compiling JSONIMPEX17 ******
echo ******                       ******
echo

echo git clone jsonimpex17...
git clone https://bitbucket.org/gems4/jsonimpex17.git
cd jsonimpex17

echo "Configuring..."
cmake -G"Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH="%CONDA_PREFIX%\Library" -DBuildExamples=OFF  -DBuildTests=OFF .. -A x64 -S . -B build
echo "Building..."
cmake --build build --target install


cd ..\..

REM Housekeeping
rd /s /q tmp_velo
