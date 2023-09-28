for file in $(find . -name 'Plugin.hpp')
do
  mv $file $(echo "$file" | sed -r 's|Plugin|plugin|g')
done



