source ~/.bashrc
work_path=$(dirname $0)
cd ${work_path}
abs_path=`pwd`
echo ${abs_path}

mkdir ${abs_path}/build
cd ${abs_path}/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=MUZJLS45FE -DFT_DISABLE_HARFBUZZ=TRUE -DDCMTK_NO_TRY_RUN:BOOL=TRUE -DDCMTK_ICONV_FLAGS_ANALYZED:BOOL=TRUE -DDCMTK_STDLIBC_ICONV_HAS_DEFAULT_ENCODING:BOOL=TRUE

make -j8

cd ..
