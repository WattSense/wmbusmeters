/*
 Copyright (C) 2017-2020 Fredrik Öhrström

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dvparser.h"
#include "meters.h"
#include "meters_common_implementation.h"
#include "util.h"
#include "wmbus.h"
#include "wmbus_utils.h"

#include <cstring>
#include <assert.h>

using namespace std;

enum DataKind {
  Str,
  Num,
};

union DataValue {
  string str;
  double num;

  // empty constructor/destructor to avoid missing constructor/destructor error
  DataValue() {}
  ~DataValue() {}
  DataValue& operator=(const DataValue& other) {
    memcpy(this, &other, sizeof(DataValue));
    return *this;
  }
};

struct Data {
  DataKind kind_;
  DataValue value_;

  Data() : Data(0.0f) {}

  Data(string& s) {
    value_.str = s;
    kind_ = DataKind::Str;
  }

  Data(double num) {
    value_.num = num;
    kind_ = DataKind::Num;
  }

  Data& operator=(const Data& other) {
    this->kind_ = other.kind_;
    this->value_ = other.value_;
    return *this;
  }
};

struct MeterGeneric : public virtual GenericMeter,
                      public virtual MeterCommonImplementation {
  MeterGeneric(MeterInfo &mi);

  virtual bool isTelegramForMe(Telegram *t);

private:
  void processContent(Telegram *t);
  void clearData();
  void addData(const string& drh, string value);
  void addData(const string& drh, double value);
  const Data& getData(const string& k);

  map<string, Data> drhData;
  bool has_mfct; // manufacturer
  int mfct_id;
  bool has_version;
  uchar version;
  bool has_devtype;
  uchar devtype;
};

shared_ptr<GenericMeter> createGeneric(MeterInfo &mi)
{
    return shared_ptr<GenericMeter>(new MeterGeneric(mi));
}

MeterGeneric::MeterGeneric(MeterInfo &mi) : MeterCommonImplementation(mi, MeterType::GENERIC) {
  vector<uchar> tmp;
  has_mfct = !mi.manufacturer_id.empty();
  has_version = !mi.version.empty();
  has_devtype = !mi.device_type.empty();
  if (has_mfct) {
    hex2bin(mi.manufacturer_id, &tmp);
    mfct_id = (tmp[0] << 8) + tmp[1];
    tmp.clear();
  }
  if (has_version) {
    hex2bin(mi.version, &tmp);
    version = tmp[0];
    tmp.clear();
  }
  if (has_devtype) {
    hex2bin(mi.device_type, &tmp);
    devtype = tmp[0];
  }
}

void MeterGeneric::clearData() {
  drhData.clear();
}

void MeterGeneric::addData(const string& drh, string value) {
  drhData[drh] = value;
}

void MeterGeneric::addData(const string& drh, double value) {
  drhData[drh] = value;
}

const Data& MeterGeneric::getData(const string& k) {
  return drhData[k];
}

void MeterGeneric::processContent(Telegram* t) {
  prints_.clear();
  clearData();
  map<string, pair<int, DVEntry>> &dvEntries = t->values;
  for (auto dvEntry: dvEntries) {
    const string& key = dvEntry.first;
    int offset;
    double number;
    std::string str;

    if (extractDVdouble(&dvEntries, key, &offset, &number, false)) {
      // capture local key variable by value since lambda is called
      // after this function ended, which means after local variable's
      // lifetime ended
      addPrint(key, Quantity::Other, [number](Unit u) { return number; }, key, true, true);
    } else if (extractDVstring(&dvEntries, key, &offset, &str)) {
      // capture local key variable by value since lambda is called
      // after this function ended, which means after local variable's
      // lifetime ended
      addPrint(key, Quantity::Other, [str]() { return str; }, key, true, true);
    } else {
      verbose("(meter_generic) ignoring drh %s\n", key.c_str());
      continue;
    }
  }
}


bool MeterGeneric::isTelegramForMe(Telegram *t) {
  if (has_mfct && mfct_id != t->dll_mfct) {
    return false;
  }
  if (has_devtype && devtype != t->dll_type) {
    return false;
  }
  if (has_version && version != t->dll_version) {
    return false;
  }
  return doesIdMatchExpressions(t->id, ids_);
}
