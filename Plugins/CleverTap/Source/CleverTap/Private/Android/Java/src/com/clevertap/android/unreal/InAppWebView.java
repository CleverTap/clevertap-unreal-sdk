package com.clevertap.android.unreal;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Point;
import android.graphics.Insets;
import android.os.Build;
import android.util.TypedValue;
import android.view.WindowInsets;
import android.view.WindowManager;
import android.view.WindowMetrics;
import android.webkit.WebSettings;
import android.webkit.WebView;
import androidx.annotation.Px;
import androidx.annotation.RequiresApi;
import com.clevertap.android.sdk.CTWebInterface;

// A re-implementation of com.clevertap.android.sdk.inapp.CTInAppWebView which unfortunately is an internal class.
public final class InAppWebView extends WebView {

    // Constructors
    // -------------

    @SuppressLint({ "ViewConstructor", "SetJavaScriptEnabled" })
    public InAppWebView(
            Context ctx,
            int widthDp,
            int heightDp,
            int widthPct,
            int heightPct,
            double aspectRatio) {

        super(ctx);
        this.context = ctx;
        this.widthDp = widthDp;
        this.heightDp = heightDp;
        this.widthPercentage = widthPct;
        this.heightPercentage = heightPct;
        this.aspectRatio = aspectRatio <= 0.0 ? DEFAULT_ASPECT_RATIO : aspectRatio;

        // view flags
        setHorizontalScrollBarEnabled(false);
        setVerticalScrollBarEnabled(false);
        setHorizontalFadingEdgeEnabled(false);
        setVerticalFadingEdgeEnabled(false);
        setOverScrollMode(OVER_SCROLL_NEVER);
        setBackgroundColor(0x00000000);

        // ignore user font-size changes
        getSettings().setTextZoom(100);
        setId(188293);
    }

    public InAppWebView(
            Context ctx,
            int widthDp,
            int heightDp,
            int widthPct,
            int heightPct) {
        this(ctx, widthDp, heightDp, widthPct, heightPct, DEFAULT_ASPECT_RATIO);
    }

    // Public interface
    // ------------------------------------------------------------------

    @SuppressLint("SetJavaScriptEnabled")
    public void setJavaScriptInterface(CTWebInterface iface) {
        WebSettings s = getSettings();
        s.setJavaScriptEnabled(true);
        s.setJavaScriptCanOpenWindowsAutomatically(false);
        s.setAllowContentAccess(false);
        s.setAllowFileAccess(false);
        s.setAllowFileAccessFromFileURLs(false);

        addJavascriptInterface(iface, JAVASCRIPT_INTERFACE_NAME);
    }

    public void cleanup(boolean jsEnabled) {
        removeAllViews();
        destroyDrawingCache();
        loadUrl("about:blank");
        if (jsEnabled)
            removeJavascriptInterface(JAVASCRIPT_INTERFACE_NAME);
        clearHistory();
        destroy();
    }

    @Override
    protected void onMeasure(int wSpec, int hSpec) {
        super.onMeasure(wSpec, hSpec);
        updateDimension();
        setMeasuredDimension(dim.x, dim.y);
    }

    // Private internals
    // ------------------------------------------------------------------

    private final Context context;
    private final int widthDp, heightDp;
    private final int widthPercentage, heightPercentage;
    private final double aspectRatio;
    public final Point dim = new Point();

    private static final double DEFAULT_ASPECT_RATIO = -1.0;
    private static final String JAVASCRIPT_INTERFACE_NAME = "CleverTap";

    private void updateDimension() {
        int w = (widthDp > 0) ? dpToPx(widthDp) : calculatePercentageWidth();
        int h;

        if (heightDp > 0) {
            h = dpToPx(heightDp);
        } else if (aspectRatio != DEFAULT_ASPECT_RATIO) {
            h = (int) (w / aspectRatio);
        } else {
            h = calculatePercentageHeight();
        }

        dim.x = w;
        dim.y = h;
    }

    // ---- percentage helpers -----------------------------------------

    @Px
    private int calculatePercentageWidth() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
            return calculateWidthWithWindowMetrics();
        return calculateWidthWithDisplayMetrics();
    }

    @Px
    private int calculatePercentageHeight() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R)
            return calculateHeightWithWindowMetrics();
        return calculateHeightWithDisplayMetrics();
    }

    @RequiresApi(Build.VERSION_CODES.R)
    @Px
    private int calculateWidthWithWindowMetrics() {
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        if (wm == null)
            return calculateWidthWithDisplayMetrics();

        WindowMetrics m = wm.getCurrentWindowMetrics();
        Insets in = m.getWindowInsets().getInsetsIgnoringVisibility(
                WindowInsets.Type.systemBars() | WindowInsets.Type.displayCutout());
        int avail = m.getBounds().width() - in.left - in.right;
        return (int) (avail * widthPercentage / 100f);
    }

    @RequiresApi(Build.VERSION_CODES.R)
    @Px
    private int calculateHeightWithWindowMetrics() {
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        if (wm == null)
            return calculateHeightWithDisplayMetrics();

        WindowMetrics m = wm.getCurrentWindowMetrics();
        Insets in = m.getWindowInsets().getInsetsIgnoringVisibility(
                WindowInsets.Type.systemBars() | WindowInsets.Type.displayCutout());
        int avail = m.getBounds().height() - in.top - in.bottom;
        return (int) (avail * heightPercentage / 100f);
    }

    @Px
    private int calculateWidthWithDisplayMetrics() {
        return (int) (context.getResources().getDisplayMetrics().widthPixels
                * widthPercentage / 100f);
    }

    @Px
    private int calculateHeightWithDisplayMetrics() {
        return (int) (context.getResources().getDisplayMetrics().heightPixels
                * heightPercentage / 100f);
    }

    // ---- util --------------------------------------------------------

    @Px
    private int dpToPx(int dp) {
        return (int) TypedValue.applyDimension(
                TypedValue.COMPLEX_UNIT_DIP, dp,
                getResources().getDisplayMetrics());
    }
}
