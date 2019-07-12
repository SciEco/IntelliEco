package cc.hurrypeng.intellieco;

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
}
