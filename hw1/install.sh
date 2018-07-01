git clone https://bitbucket.org/chengyinwu/v3.git
rm -rf v3/engine
cp -rf SoCV_v3_engine v3/engine
cd v3/engine/; ./boolector.script; ./minisat.script; ./quteRTL.script; cd ../; make;
