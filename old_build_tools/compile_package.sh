./compile.sh || {
    echo "**compilation failed**"
    exit 1
}

./create_ipa.sh || {
     echo "**IPA creation failed**"
     exit 1
}
