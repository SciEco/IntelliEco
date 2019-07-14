package cc.hurrypeng.intellieco;

import java.util.ArrayList;
import java.util.List;

class DataPack {

    static class Request {
        String username;
        String password;
        String request;
    }

    static class Response {
        String authority;
    }

    static class LoginRequest extends Request {
    }

    static class LoginResponse extends Response{
    }

    static class UploadRequest extends Request {
        long time;
        double longtitude;
        double latitude;
        String imageFilename;
    }

    static class UploadResponse extends Response {
        int mothCount;
    }

    static class RefreshRequest extends Request {
    }

    static class Record {
        String uploader;
        long time;
        double longtitude;
        double latitude;
        int mothCount;
    }

    static class RefreshResponse extends Response {
        List<Record> records;
    }
}
