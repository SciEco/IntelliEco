package cc.hurrypeng.intellieco;

import android.os.Handler;
import android.os.Message;

import org.apache.commons.net.ftp.FTP;
import org.apache.commons.net.ftp.FTPClient;
import org.apache.commons.net.ftp.FTPReply;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.UUID;

class FTPTask extends Thread {
    final static int RESPONSE_RECV = 1;

    private Handler handler;
    private File response;
    private ArrayList<File> sends;

    private String server;
    private String username;
    private String password;
    private String uuid;

    public FTPTask(Handler handler, File response, File... send) {
        super();
        this.handler = handler;
        this.response = response;
        server = "ie.hurrypeng.cc";
        username = "IE_User";
        password = "PKUSZE410";
        uuid = UUID.randomUUID().toString().replaceAll("-", "");
        sends = new ArrayList<File>();
        for (File file : send) sends.add(file);
    }

    @Override
    public void run() {
        FTPClient ftp = new FTPClient();
        try {
            int reply;
            ftp.connect(server);
            // After connection attempt, you should check the reply code to verify
            // success.
            reply = ftp.getReplyCode();
            if (!FTPReply.isPositiveCompletion(reply)) {
                ftp.disconnect();
                return;
            }

            if(!ftp.login(username, password)) {
                return;
            }

            ftp.enterLocalPassiveMode();
            ftp.setRemoteVerificationEnabled(false);

            ftp.makeDirectory(uuid);
            ftp.changeWorkingDirectory(uuid);
            ftp.setFileType(FTP.BINARY_FILE_TYPE);

            for (File file : sends) {
                String filename = file.getName();
                ftp.storeFile(filename, new FileInputStream(file));
            }

            ftp.makeDirectory("request");

            // Wait for response
            int count = 10;
            while (count != 0)
            {
                if (ftp.removeDirectory("response")) break;

                // Sleep
                new Thread() {
                    @Override
                    public void run() {
                        super.run();
                        try {
                            Thread.sleep(1000);
                        }
                        catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }.run();
                count--;
            }
            if (count == 0) return;

            if (response.exists()) response.delete();
            response.createNewFile();

            FileOutputStream responseStream = new FileOutputStream(response);
            ftp.retrieveFile("response.json", responseStream);
            responseStream.close();

            ftp.makeDirectory("receive");

            Message message = new Message();
            message.what = RESPONSE_RECV;
            handler.sendMessage(message);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}
