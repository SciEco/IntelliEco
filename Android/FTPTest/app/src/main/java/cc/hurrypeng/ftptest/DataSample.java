package cc.hurrypeng.ftptest;

class DataSample {
    public String username;
    public long time;
    double longtitude;
    double latitude;
    public String image;

    public DataSample()
    {
        username = null;
        time = 0;
        longtitude = latitude = 0.0;
        image = null;
    }

    public DataSample(String username, long time, double longtitude, double latitude, String image)
    {
        this.username = username;
        this.time = time;
        this.longtitude = longtitude;
        this.latitude = latitude;
        this.image = image;
    }
}
