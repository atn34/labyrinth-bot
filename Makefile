CXXFLAGS := -std=c++11 -Wall -Werror

.PHONY: all
all: create_walls_and_holes_helper_main fps_main path_main geometry_test motor_client_test ring_buffer_test

create_walls_and_holes_helper_main.o: create_walls_and_holes_helper_main.cc camera_properties.h geometry.h subgoals.h walls_and_holes.h
	$(CXX) -c create_walls_and_holes_helper_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

fps_main.o: fps_main.cc camera_properties.h
	$(CXX) -c fps_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

geometry.o: geometry.cc geometry.h
	$(CXX) -c geometry.cc $(CXXFLAGS)  -o $@

geometry_test.o: geometry_test.cc geometry.h
	$(CXX) -c geometry_test.cc $(CXXFLAGS)  -o $@

mask_util.o: mask_util.cc camera_properties.h mask_util.h
	$(CXX) -c mask_util.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

motor_client.o: motor_client.cc motor_client.h
	$(CXX) -c motor_client.cc $(CXXFLAGS)  -o $@

motor_client_test.o: motor_client_test.cc motor_client.h
	$(CXX) -c motor_client_test.cc $(CXXFLAGS)  -o $@

path_main.o: path_main.cc bfs.h geometry.h mask_util.h
	$(CXX) -c path_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

ring_buffer_test.o: ring_buffer_test.cc ring_buffer.h
	$(CXX) -c ring_buffer_test.cc $(CXXFLAGS)  -o $@

subgoals.o: subgoals.cc bfs.h camera_properties.h geometry.h mask_util.h subgoals.h walls_and_holes.h
	$(CXX) -c subgoals.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

walls_and_holes.o: walls_and_holes.cc camera_properties.h geometry.h walls_and_holes.h
	$(CXX) -c walls_and_holes.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

create_walls_and_holes_helper_main: create_walls_and_holes_helper_main.o geometry.o mask_util.o subgoals.o walls_and_holes.o
	$(CXX) $(LDFLAGS) create_walls_and_holes_helper_main.o geometry.o mask_util.o subgoals.o walls_and_holes.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

fps_main: fps_main.o 
	$(CXX) $(LDFLAGS) fps_main.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

path_main: path_main.o geometry.o mask_util.o
	$(CXX) $(LDFLAGS) path_main.o geometry.o mask_util.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

geometry_test: geometry_test.o geometry.o
	$(CXX) $(LDFLAGS) geometry_test.o geometry.o -lgmock -lgmock_main -lpthread -o $@

motor_client_test: motor_client_test.o motor_client.o
	$(CXX) $(LDFLAGS) motor_client_test.o motor_client.o -L/usr/lib -Wl,-rpath /usr/lib -lgmock -lgmock_main -lpthread -lsocket++ -o $@

ring_buffer_test: ring_buffer_test.o 
	$(CXX) $(LDFLAGS) ring_buffer_test.o -lgmock -lgmock_main -lpthread -o $@

.PHONY: test
test: geometry_test motor_client_test ring_buffer_test
	./geometry_test
	./motor_client_test
	./ring_buffer_test

.PHONY: clean
clean:
	find . -type f -name '*.o' -delete
	rm -f create_walls_and_holes_helper_main fps_main path_main geometry_test motor_client_test ring_buffer_test
