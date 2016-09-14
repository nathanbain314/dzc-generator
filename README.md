# dzc-generator
A tool to generate deep zoom collections
#Building
[Libvips](http://www.vips.ecs.soton.ac.uk/index.php?title=Libvips) is required in order for this to build. Building is achieved by running **make**.
#Generating the collection
./build_collection &nbsp;input_directory &nbsp;output_directory &nbsp;collection_name

This will build a [deep zoom collection](https://msdn.microsoft.com/en-us/library/cc645077%28v=vs.95%29.aspx#Collections) by saving all of the images in input_directory as deep zoom images in output_directory and a zoomable collection of thumbnails with the collection_name.
