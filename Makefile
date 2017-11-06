CXXFLAGS := -std=c++11 -Wall -Werror

.PHONY: all
all: controller_main create_mask_main create_walls_and_holes_helper_main measure_ball_position_main path_main perspective_transform_main train_find_ball_gui_main tune_ball_threshold_main tune_pink_threshold_main find_pink_corners_test geometry_test motor_client_test ring_buffer_test

ball_state.o: ball_state.cc ball_state.h geometry.h
	$(CXX) -c ball_state.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

controller_main.o: controller_main.cc ball_state.h camera_properties.h geometry.h measure_ball_position.h motor_client.h perspective_transform.h ring_buffer.h subgoals.h walls_and_holes.h
	$(CXX) -c controller_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

create_mask_main.o: create_mask_main.cc camera_properties.h perspective_transform.h
	$(CXX) -c create_mask_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

create_walls_and_holes_helper_main.o: create_walls_and_holes_helper_main.cc camera_properties.h geometry.h subgoals.h walls_and_holes.h
	$(CXX) -c create_walls_and_holes_helper_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

find_pink_corners.o: find_pink_corners.cc bfs.h connected_components.h find_pink_corners.h geometry.h
	$(CXX) -c find_pink_corners.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

find_pink_corners_test.o: find_pink_corners_test.cc bfs.h connected_components.h find_pink_corners.h geometry.h
	$(CXX) -c find_pink_corners_test.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

geometry.o: geometry.cc geometry.h
	$(CXX) -c geometry.cc $(CXXFLAGS)  -o $@

geometry_test.o: geometry_test.cc geometry.h
	$(CXX) -c geometry_test.cc $(CXXFLAGS)  -o $@

mask_util.o: mask_util.cc camera_properties.h mask_util.h
	$(CXX) -c mask_util.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

measure_ball_position.o: measure_ball_position.cc bfs.h connected_components.h geometry.h measure_ball_position.h threshold_ball.h
	$(CXX) -c measure_ball_position.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

measure_ball_position_main.o: measure_ball_position_main.cc ball_state.h camera_properties.h geometry.h measure_ball_position.h perspective_transform.h
	$(CXX) -c measure_ball_position_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

motor_client.o: motor_client.cc motor_client.h
	$(CXX) -c motor_client.cc $(CXXFLAGS)  -o $@

motor_client_test.o: motor_client_test.cc motor_client.h
	$(CXX) -c motor_client_test.cc $(CXXFLAGS)  -o $@

path_main.o: path_main.cc bfs.h geometry.h mask_util.h
	$(CXX) -c path_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

perspective_transform.o: perspective_transform.cc camera_properties.h find_pink_corners.h perspective_transform.h
	$(CXX) -c perspective_transform.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

perspective_transform_main.o: perspective_transform_main.cc camera_properties.h perspective_transform.h
	$(CXX) -c perspective_transform_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

ring_buffer_test.o: ring_buffer_test.cc ring_buffer.h
	$(CXX) -c ring_buffer_test.cc $(CXXFLAGS)  -o $@

subgoals.o: subgoals.cc bfs.h camera_properties.h geometry.h mask_util.h subgoals.h walls_and_holes.h
	$(CXX) -c subgoals.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

threshold_ball.o: threshold_ball.cc mask_util.h threshold_ball.h
	$(CXX) -c threshold_ball.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

train_find_ball_gui_main.o: train_find_ball_gui_main.cc cvui.h
	$(CXX) -c train_find_ball_gui_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

tune_ball_threshold_main.o: tune_ball_threshold_main.cc camera_properties.h perspective_transform.h threshold_ball.h
	$(CXX) -c tune_ball_threshold_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

tune_pink_threshold_main.o: tune_pink_threshold_main.cc camera_properties.h find_pink_corners.h
	$(CXX) -c tune_pink_threshold_main.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

walls_and_holes.o: walls_and_holes.cc camera_properties.h geometry.h walls_and_holes.h
	$(CXX) -c walls_and_holes.cc $(CXXFLAGS) -Iopencv-3.3.0-prebuilt/include -o $@

controller_main: controller_main.o ball_state.o find_pink_corners.o geometry.o mask_util.o measure_ball_position.o motor_client.o perspective_transform.o subgoals.o threshold_ball.o walls_and_holes.o
	$(CXX) $(LDFLAGS) controller_main.o ball_state.o find_pink_corners.o geometry.o mask_util.o measure_ball_position.o motor_client.o perspective_transform.o subgoals.o threshold_ball.o walls_and_holes.o -L/usr/lib -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath /usr/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -lsocket++ -o $@

create_mask_main: create_mask_main.o find_pink_corners.o geometry.o perspective_transform.o
	$(CXX) $(LDFLAGS) create_mask_main.o find_pink_corners.o geometry.o perspective_transform.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

create_walls_and_holes_helper_main: create_walls_and_holes_helper_main.o geometry.o mask_util.o subgoals.o walls_and_holes.o
	$(CXX) $(LDFLAGS) create_walls_and_holes_helper_main.o geometry.o mask_util.o subgoals.o walls_and_holes.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

measure_ball_position_main: measure_ball_position_main.o ball_state.o find_pink_corners.o geometry.o mask_util.o measure_ball_position.o perspective_transform.o threshold_ball.o
	$(CXX) $(LDFLAGS) measure_ball_position_main.o ball_state.o find_pink_corners.o geometry.o mask_util.o measure_ball_position.o perspective_transform.o threshold_ball.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

path_main: path_main.o geometry.o mask_util.o
	$(CXX) $(LDFLAGS) path_main.o geometry.o mask_util.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

perspective_transform_main: perspective_transform_main.o find_pink_corners.o geometry.o perspective_transform.o
	$(CXX) $(LDFLAGS) perspective_transform_main.o find_pink_corners.o geometry.o perspective_transform.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

train_find_ball_gui_main: train_find_ball_gui_main.o 
	$(CXX) $(LDFLAGS) train_find_ball_gui_main.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

tune_ball_threshold_main: tune_ball_threshold_main.o find_pink_corners.o geometry.o mask_util.o perspective_transform.o threshold_ball.o
	$(CXX) $(LDFLAGS) tune_ball_threshold_main.o find_pink_corners.o geometry.o mask_util.o perspective_transform.o threshold_ball.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

tune_pink_threshold_main: tune_pink_threshold_main.o find_pink_corners.o geometry.o
	$(CXX) $(LDFLAGS) tune_pink_threshold_main.o find_pink_corners.o geometry.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -o $@

find_pink_corners_test: find_pink_corners_test.o find_pink_corners.o geometry.o
	$(CXX) $(LDFLAGS) find_pink_corners_test.o find_pink_corners.o geometry.o -Lopencv-3.3.0-prebuilt/lib -Wl,-rpath opencv-3.3.0-prebuilt/lib -lgmock -lgmock_main -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videoio -lopencv_videostab -lpthread -o $@

geometry_test: geometry_test.o geometry.o
	$(CXX) $(LDFLAGS) geometry_test.o geometry.o -lgmock -lgmock_main -lpthread -o $@

motor_client_test: motor_client_test.o motor_client.o
	$(CXX) $(LDFLAGS) motor_client_test.o motor_client.o -L/usr/lib -Wl,-rpath /usr/lib -lgmock -lgmock_main -lpthread -lsocket++ -o $@

ring_buffer_test: ring_buffer_test.o 
	$(CXX) $(LDFLAGS) ring_buffer_test.o -lgmock -lgmock_main -lpthread -o $@

.PHONY: test
test: find_pink_corners_test geometry_test motor_client_test ring_buffer_test
	./find_pink_corners_test
	./geometry_test
	./motor_client_test
	./ring_buffer_test

.PHONY: clean
clean:
	find . -type f -name '*.o' -delete
	rm -f controller_main create_mask_main create_walls_and_holes_helper_main measure_ball_position_main path_main perspective_transform_main train_find_ball_gui_main tune_ball_threshold_main tune_pink_threshold_main find_pink_corners_test geometry_test motor_client_test ring_buffer_test
