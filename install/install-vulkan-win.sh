mkdir -p vulkan
cd vulkan
json=$(wget -qO- https://vulkan.lunarg.com/sdk/files.json)
versions=$(echo $json | jq ".windows | keys")
echo 'Here are available Vulkan SDK versions :'
echo "$versions" | jq ".[]" | sed 's/"//g' | while read line; do
    echo "$line"
done
ver=
while [ "$ver" = "" ]; do
    read -p "Enter desired SDK version : " ver
done
data=$(echo $json | jq ".windows.\"$ver\".files[0]")
filename=$(echo $data | jq ".file_name" | sed "s/\"//g")
if echo "$versions" | jq -e "any(. == \"$ver\") | not">/dev/null; then
    echo 'Could not find version in list !'
    exit 1
fi
if ! test -f "$filename"; then
    echo "Downloading..."
    wget "https://sdk.lunarg.com/sdk/download/$ver""/windows/$filename"
fi
if ! echo "$(echo "$data" | jq ".sha" | sed "s/\"//g") $filename" | sha256sum -c --status; then
    echo "Wrong file downloaded ! Try again !"
    exit 1
fi
echo "Installing..."
./$filename