package org.libreoffice;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.widget.DrawerLayout;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.mozilla.gecko.ZoomConstraints;
import org.mozilla.gecko.gfx.GeckoLayerClient;
import org.mozilla.gecko.gfx.LayerController;
import org.mozilla.gecko.gfx.LayerView;

import java.util.ArrayList;
import java.util.List;

public class LibreOfficeMainActivity extends Activity {

    private static final String LOGTAG = "LibreOfficeMainActivity";
    private static final String DEFAULT_DOC_PATH = "/assets/example.odt";

    public static LibreOfficeMainActivity mAppContext;

    private static LayerController mLayerController;
    private static GeckoLayerClient mLayerClient;
    private static LOKitThread sLOKitThread;

    public Handler mMainHandler;

    private DrawerLayout mDrawerLayout;
    private RelativeLayout mGeckoLayout;
    private ListView mDrawerList;
    private List<DocumentPartView> mDocumentPartView = new ArrayList<DocumentPartView>();
    private DocumentPartViewListAdpater mDocumentPartViewListAdpater;
    private String mInputFile;

    public static GeckoLayerClient getLayerClient() {
        return mLayerClient;
    }

    public static LayerController getLayerController() {
        return mLayerController;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.action_about:
                showAbout();
                return true;
            case R.id.action_parts:
                mDrawerLayout.openDrawer(mDrawerList);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        // If the nav drawer is open, hide action items related to the content view
        boolean isDrawerOpen = mDrawerLayout.isDrawerOpen(mDrawerList);
        menu.findItem(R.id.action_parts).setVisible(!isDrawerOpen);
        return super.onPrepareOptionsMenu(menu);
    }

    public DisplayMetrics getDisplayMetrics() {
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        return metrics;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.w(LOGTAG, "onCreate..");
        mAppContext = this;
        super.onCreate(savedInstanceState);

        mMainHandler = new Handler();

        LayoutInflater.from(this).setFactory(ViewFactory.getInstance());

        if (getIntent().getData() != null) {
            mInputFile = getIntent().getData().getEncodedPath();
        } else {
            mInputFile = DEFAULT_DOC_PATH;
        }

        setContentView(R.layout.activity_main);

        getActionBar().setDisplayHomeAsUpEnabled(false);
        getActionBar().setHomeButtonEnabled(false);

        mGeckoLayout = (RelativeLayout) findViewById(R.id.gecko_layout);
        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        if (mDocumentPartViewListAdpater == null) {
            mDrawerList = (ListView) findViewById(R.id.left_drawer);

            mDocumentPartViewListAdpater = new DocumentPartViewListAdpater(this, R.layout.document_part_list_layout, mDocumentPartView);
            mDrawerList.setAdapter(mDocumentPartViewListAdpater);
            mDrawerList.setOnItemClickListener(new DocumentPartClickListener());
        }

        if (sLOKitThread == null) {
            sLOKitThread = new LOKitThread();
            sLOKitThread.start();
        } else {
            sLOKitThread.clearQueue();
        }

        mLayerController = new LayerController(this);
        mLayerController.setZoomConstraints(new ZoomConstraints(true));
        mLayerClient = new GeckoLayerClient(this);
        LayerView layerView = (LayerView) findViewById(R.id.layer_view);
        mLayerController.setView(layerView);
        mLayerController.setLayerClient(mLayerClient);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i(LOGTAG, "onResume..");
    }

    @Override
    protected void onPause() {
        Log.i(LOGTAG, "onPause..");
        super.onPause();
    }

    @Override
    protected void onStart() {
        Log.i(LOGTAG, "onStart..");
        super.onStart();
        LOKitShell.sendEvent(LOEventFactory.load(mInputFile));
    }

    @Override
    protected void onStop() {
        Log.i(LOGTAG, "onStop..");
        LOKitShell.sendEvent(LOEventFactory.close());
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        Log.i(LOGTAG, "onDestroy..");
        super.onDestroy();
    }

    public LOKitThread getLOKitThread() {
        return sLOKitThread;
    }

    public List<DocumentPartView> getDocumentPartView() {
        return mDocumentPartView;
    }

    public DocumentPartViewListAdpater getDocumentPartViewListAdpater() {
        return mDocumentPartViewListAdpater;
    }

    private void showAbout() {
        // Inflate the about message contents
        View messageView = getLayoutInflater().inflate(R.layout.about, null, false);

        // When linking text, force to always use default color. This works
        // around a pressed color state bug.
        TextView textView = (TextView) messageView.findViewById(R.id.about_credits);
        int defaultColor = textView.getTextColors().getDefaultColor();
        textView.setTextColor(defaultColor);

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setIcon(R.drawable.lo_icon);
        builder.setTitle(R.string.app_name);
        builder.setView(messageView);

        builder.setNegativeButton(R.string.about_license, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int id) {
                LOKitShell.sendEvent(LOEventFactory.close());
                LOKitShell.sendEvent(LOEventFactory.load("/assets/license.txt"));
                dialog.dismiss();
            }
        });

        builder.setPositiveButton(R.string.about_notice, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int id) {
                LOKitShell.sendEvent(LOEventFactory.close());
                LOKitShell.sendEvent(LOEventFactory.load("/assets/notice.txt"));
                dialog.dismiss();
            }
        });

        AlertDialog dialog = builder.create();
        dialog.show();
    }

    public void showProgressSpinner() {
        findViewById(R.id.loadingPanel).setVisibility(View.VISIBLE);
    }

    public void hideProgressSpinner() {
        findViewById(R.id.loadingPanel).setVisibility(View.GONE);
    }

    public void showAlertDialog(String s) {

        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(LibreOfficeMainActivity.this);

        alertDialogBuilder.setTitle("Error");
        alertDialogBuilder.setMessage(s);
        alertDialogBuilder.setNeutralButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                finish();
            }
        });

        AlertDialog alertDialog = alertDialogBuilder.create();
        alertDialog.show();
    }

    private class DocumentPartClickListener implements android.widget.AdapterView.OnItemClickListener {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            DocumentPartView partView = mDocumentPartViewListAdpater.getItem(position);
            LOKitShell.sendEvent(LOEventFactory.changePart(partView.getPartIndex()));
            mDrawerLayout.closeDrawer(mDrawerList);
        }
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
