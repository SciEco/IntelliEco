package cc.hurrypeng.ftptest;

class DataSample {
    public String username;
    public long time;
    double longtitude;
    double latitude;
    public String imageFilename;

    public DataSample()
    {
        username = null;
        time = 0;
        longtitude = latitude = 0.0;
        imageFilename = null;
    }

    public DataSample(String username, long time, double longtitude, double latitude, String imageFilename)
    {
        this.username = username;
        this.time = time;
        this.longtitude = longtitude;
        this.latitude = latitude;
        this.imageFilename = imageFilename;
    }
}
