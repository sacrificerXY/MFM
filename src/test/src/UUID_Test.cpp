#include "assert.h"
#include "UUID_Test.h"
#include "CharBufferByteSink.h"
#include "ZStringByteSource.h"
#include <string.h>        /* For memcpy */

namespace MFM {
  typedef CharBufferByteSink<1024> CBS1K;

  static CBS1K tbuf;
  static ZStringByteSource fbuf("");

  static void Test_Basic() {
    UUID u1("hi",1,2,3);

    assert(!strcmp(u1.GetLabel(),"hi"));
    assert(u1.GetVersion() == 1);
    assert(u1.GetHexDate() == 2);
    assert(u1.GetHexTime() == 3);

    UUID u2("hi",1,0x20140516,0x190447);
    assert(!strcmp(u2.GetLabel(),"hi"));
    assert(u2.GetVersion() == 1);
    assert(u2.GetHexDate() == 0x20140516);
    assert(u2.GetHexTime() == 0x190447);

    assert(u1.CompatibleLabel(u2));
    assert(u2.CompatibleLabel(u1));
    assert(u1.CompatibleLabel(u1));
    assert(u2.CompatibleLabel(u2));

    assert(u1.CompatibleAPIVersion(u2));
    assert(u2.CompatibleAPIVersion(u1));
    assert(u1.CompatibleAPIVersion(u1));
    assert(u2.CompatibleAPIVersion(u2));

    assert(!u1.CompatibleButStrictlyNewer(u2));
    assert(!u1.CompatibleButStrictlyNewer(u1));
    assert(u2.CompatibleButStrictlyNewer(u1));
    assert(!u2.CompatibleButStrictlyNewer(u2));

    UUID u3("hi",2,0x20140516,0x191339);
    assert(u1.CompatibleLabel(u3));
    assert(u2.CompatibleLabel(u3));
    assert(u3.CompatibleLabel(u3));

    assert(!u3.CompatibleAPIVersion(u1));
    assert(!u3.CompatibleAPIVersion(u2));
    assert(u3.CompatibleAPIVersion(u3));

    assert(!u3.CompatibleButStrictlyNewer(u1));
    assert(!u3.CompatibleButStrictlyNewer(u2));
    assert(!u3.CompatibleButStrictlyNewer(u3));

  }

  static void Test_Vprintf_Result(const char * result, const char * format, ... ) {
    va_list ap;
    tbuf.Reset();
    va_start(ap, format);
    tbuf.Vprintf(format, ap);
    va_end(ap);
    assert(tbuf.Equals(result));
  }

  static void Test_Print() {
    UUID u1("Sorter",1,2,3);
    Test_Vprintf_Result("foo[6Sorter118000000026000003]bar", "foo[%@]bar", &u1);
    Test_Vprintf_Result("([6Sorter118000000026000003])", "([%#@])", 1234, &u1);

    UUID u2("hi",2,0x20140516,0x191339);
    Test_Vprintf_Result("foo/2hi128201405166191339/bar", "foo/%@/bar", &u2);

    UUID u3("Vacuous Snare Drum, with Kickback Reburn",
            87, 0x20140516, 0x214811);
    Test_Vprintf_Result("[9240Vacuous Snare Drum, with Kickback Reburn2878201405166214811]",
                        "[%@]", &u3);

    UUID u4("", 1, 2, 3);
    Test_Vprintf_Result("foo0118000000026000003bar", "foo%@bar", &u4);
  }

  static void Test_Vprintf_Read(const UUID & u) {
    tbuf.Reset();
    tbuf.Printf("%@", &u);
    fbuf.Reset(tbuf.GetZString());
    UUID r(fbuf);
    assert(u == r);
    assert(fbuf.Read() == -1);
  }

  static void Test_Read() {
    Test_Vprintf_Read(UUID("Sorter",1,2,3));
    Test_Vprintf_Read(UUID("hi",2,0x20140516,0x191339));
    Test_Vprintf_Read(UUID("Vacuous Snare Drum, with Kickback Reburn",
                           87, 0x20140516, 0x214811));
    Test_Vprintf_Read(UUID("", 1, 2, 3));
  }

  void UUID_Test::Test_RunTests() {
    Test_Basic();
    Test_Print();
    Test_Read();
  }

} /* namespace MFM */

