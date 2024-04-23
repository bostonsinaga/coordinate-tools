class Point {
    Point() {}

    Point(Point pt_in) {
        lat = pt_in.lat;
        lng = pt_in.lng;
    }

    Point(double lat_in, double lng_in) {
        lat = lat_in;
        lng = lng_in;
    }

    double lat = 0, lng = 0;
};

