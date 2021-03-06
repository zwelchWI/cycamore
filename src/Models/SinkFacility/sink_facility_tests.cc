// sink_facility_tests.cc
#include <gtest/gtest.h>

#include "facility_model_tests.h"
#include "model_tests.h"
#include "resource_helpers.h"
#include "xml_query_engine.h"
#include "xml_parser.h"

#include "sink_facility_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacilityTest::SetUp() {
  InitParameters();
  SetUpSinkFacility();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacilityTest::TearDown() {
  delete src_facility;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacilityTest::InitParameters() {
  commod1_ = "acommod";
  commod2_ = "bcommod";
  commod3_ = "ccommod";
  capacity_ = 5;
  inv_ = capacity_ * 2;
  qty_ = capacity_ * 0.5;
  ncommods_ = 2;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SinkFacilityTest::SetUpSinkFacility() {
  using cycamore::SinkFacility;
  src_facility = new SinkFacility(tc_.get());
  src_facility->AddCommodity(commod1_);
  src_facility->AddCommodity(commod2_);
  src_facility->capacity(capacity_);
  src_facility->SetMaxInventorySize(inv_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, InitialState) {
  EXPECT_DOUBLE_EQ(0.0, src_facility->InventorySize());
  EXPECT_DOUBLE_EQ(capacity_, src_facility->capacity());
  EXPECT_DOUBLE_EQ(inv_, src_facility->MaxInventorySize());
  EXPECT_DOUBLE_EQ(capacity_, src_facility->RequestAmt());
  EXPECT_DOUBLE_EQ(0.0, src_facility->InventorySize());
  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> vexp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  EXPECT_EQ(vexp, src_facility->input_commodities());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, Clone) {
  using cycamore::SinkFacility;
  SinkFacility* cloned_fac = dynamic_cast<cycamore::SinkFacility*>
                             (src_facility->Clone());

  EXPECT_DOUBLE_EQ(0.0, cloned_fac->InventorySize());
  EXPECT_DOUBLE_EQ(capacity_, cloned_fac->capacity());
  EXPECT_DOUBLE_EQ(inv_, cloned_fac->MaxInventorySize());
  EXPECT_DOUBLE_EQ(capacity_, cloned_fac->RequestAmt());
  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> vexp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  EXPECT_EQ(vexp, cloned_fac->input_commodities());

  delete cloned_fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, XMLInit) {
  std::stringstream ss;
  ss << "<root>" << "<input>"
     << "<commodities>"
     << "<incommodity>" << commod1_ << "</incommodity>"
     << "<incommodity>" << commod2_ << "</incommodity>"
     << "</commodities>"
     << "<input_capacity>" << capacity_ << "</input_capacity>"
     << "<inventorysize>" << inv_ << "</inventorysize>"
     << "</input>" << "</root>";

  cyclus::XMLParser p;
  p.Init(ss);
  cyclus::XMLQueryEngine engine(p);
  cycamore::SinkFacility fac(tc_.get());

  EXPECT_NO_THROW(fac.InitModuleMembers(&engine););
  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> vexp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  EXPECT_EQ(vexp, fac.input_commodities());
  EXPECT_DOUBLE_EQ(capacity_, fac.capacity());
  EXPECT_DOUBLE_EQ(inv_, fac.MaxInventorySize());
  EXPECT_DOUBLE_EQ(capacity_, fac.RequestAmt());
  EXPECT_DOUBLE_EQ(0.0, fac.InventorySize());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, Requests) {
  using cyclus::Request;
  using cyclus::RequestPortfolio;
  using cyclus::CapacityConstraint;
  using cyclus::Material;

  std::string arr[] = {commod1_, commod2_};
  std::vector<std::string> commods (arr, arr + sizeof(arr) / sizeof(arr[0]) );
  
  std::set<RequestPortfolio<Material>::Ptr> ports =
      src_facility->GetMatlRequests();

  ASSERT_EQ(ports.size(), 1);
  ASSERT_EQ(ports.begin()->get()->qty(), capacity_);
  const std::vector<typename Request<Material>::Ptr>& requests =
      ports.begin()->get()->requests();
  ASSERT_EQ(requests.size(), 2);

  for (int i = 0; i < ncommods_; ++i) {
    Request<Material>::Ptr req = *(requests.begin() + i);
    EXPECT_EQ(req->requester(), src_facility);
    EXPECT_EQ(req->commodity(), commods[i]);
  }

  const std::set< CapacityConstraint<Material> >& constraints =
      ports.begin()->get()->constraints();
  ASSERT_TRUE(constraints.size() > 0);
  EXPECT_EQ(constraints.size(), 1);
  EXPECT_EQ(*constraints.begin(), CapacityConstraint<Material>(capacity_));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, EmptyRequests) {
  using cyclus::Material;
  using cyclus::RequestPortfolio;

  src_facility->capacity(0);
  std::set<RequestPortfolio<Material>::Ptr> ports =
      src_facility->GetMatlRequests();
  EXPECT_TRUE(ports.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, Accept) {
  using cyclus::Bid;
  using cyclus::Material;
  using cyclus::Request;
  using cyclus::Trade;
  using test_helpers::trader;
  using test_helpers::get_mat;

  double qty = qty_ * 2;
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
                         cyclus::Material::Ptr> > responses;

  Request<Material>::Ptr req1 =
      Request<Material>::Create(get_mat(92235, qty_), src_facility, commod1_);
  Bid<Material>::Ptr bid1 = Bid<Material>::Create(req1, get_mat(), &trader);

  Request<Material>::Ptr req2 =
      Request<Material>::Create(get_mat(92235, qty_), src_facility, commod2_);
  Bid<Material>::Ptr bid2 =
      Bid<Material>::Create(req2, get_mat(92235, qty_), &trader);

  Trade<Material> trade1(req1, bid1, qty_);
  responses.push_back(std::make_pair(trade1, get_mat(92235, qty_)));
  Trade<Material> trade2(req2, bid2, qty_);
  responses.push_back(std::make_pair(trade2, get_mat(92235, qty_)));

  EXPECT_DOUBLE_EQ(0.0, src_facility->InventorySize());
  src_facility->AcceptMatlTrades(responses);  
  EXPECT_DOUBLE_EQ(qty, src_facility->InventorySize());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SinkFacilityModelConstructor(cyclus::Context* ctx) {
  using cycamore::SinkFacility;
  return dynamic_cast<cyclus::Model*>(new SinkFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityModel* SinkFacilityConstructor(cyclus::Context* ctx) {
  using cycamore::SinkFacility;
  return dynamic_cast<cyclus::FacilityModel*>(new SinkFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(SinkFac, FacilityModelTests,
                        Values(&SinkFacilityConstructor));
INSTANTIATE_TEST_CASE_P(SinkFac, ModelTests,
                        Values(&SinkFacilityModelConstructor));


