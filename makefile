build_collection:
	g++ `pkg-config vips-cpp --cflags --libs` dzc-generator.c++ -o build_collection

clean:
	rm build_collection
