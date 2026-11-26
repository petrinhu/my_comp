#include <gtest/gtest.h>
#include "../../src/presentation/viewmodels/url_helper.h"

#include <QUrl>

using my_comp::presentation::viewmodels::UrlHelper;

// UX-02a: diálogos de arquivo devolvem URL (file://, percent-encoded). A conversão para
// caminho local é do QUrl, não de replace()+decodeURIComponent() espalhados no QML.
TEST(UrlHelperTest, ConvertsFileUrlToLocalPathDecodingOnce) {
    UrlHelper helper;
    EXPECT_EQ(helper.toLocalPath(QUrl("file:///tmp/My%20Dir/a%C3%A7%C3%A3o")), QString("/tmp/My Dir/ação"));
    // '%' literal no nome ("a%41b") chega como %2541 e deve virar "%41", não "A".
    EXPECT_EQ(helper.toLocalPath(QUrl("file:///tmp/a%2541b")), QString("/tmp/a%41b"));
}

TEST(UrlHelperTest, NonLocalUrlBecomesEmpty) {
    UrlHelper helper;
    EXPECT_TRUE(helper.toLocalPath(QUrl("https://example.com/x")).isEmpty());
    EXPECT_TRUE(helper.toLocalPath(QUrl()).isEmpty());
}

// Caminho já local (texto digitado) passa intacto.
TEST(UrlHelperTest, PlainPathIsKept) {
    UrlHelper helper;
    EXPECT_EQ(helper.toLocalPath(QUrl::fromLocalFile("/usr/bin/true")), QString("/usr/bin/true"));
}
