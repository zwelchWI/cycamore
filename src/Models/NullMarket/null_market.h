// null_market.h
#if !defined(_NULLMARKET_H)
#define _NULLMARKET_H

#include <map>
#include <deque>

#include "context.h"
#include "market_model.h"

namespace cycamore {

/**
   The NullMarket class inherits from the cyclus::MarketModel class and is
   dynamically loaded by the Model class when requested.

   This market will take a set of requests and match the biggest
   requests with the biggest offers first.

   @section intro Introduction
   The NullMarket is a market type in Cyclus which matches the first
   offer with the first request for its characteristic commodity. The
   NullMarket is instantiated at the beginning of the simulation and
   exists until the end of the simulation.

   @section modelParams Model Parameters
   NullMarket behavior is comprehensively defined by the following
   parameters: - cyclus::Commodity* commod: The type of commodity for which this
   market accepts offers and requests.

   @section behavior Detailed Behavior
   The NullMarket starts operation at the beginning of the simulation
   and ends operation at the end of the simulation. It collects offers
   and requests for its characteristic commodity and matches offers and
   requests according to the simplest of algorithms, matching the first
   offerer with the first requester and setting the amount to be the
   smallest of the two. Once matches are made, the market dictates the
   matches back down to the facilities.
 */
class NullMarket : public cyclus::MarketModel {
  /* --------------------
   * all MODEL classes have these public members
   * --------------------
   */
 public:
  /**
     Default constructor
   */
  NullMarket(cyclus::Context* ctx);

  /**
     Destructor
   */
  virtual ~NullMarket();

  /* -------------------- */

  virtual cyclus::Model* Clone() {
    NullMarket* m = new NullMarket(*this);
    m->InitFrom(this);
    return m;
  }


  /* --------------------
   * all COMMUNICATOR classes have these members
   * --------------------
   */
 public:
  /**
     The market receives an offer or request that has been passed from
     the facility to the institution to the region.

     @param msg is a pointer to the message, an cyclus::Message object
   */
  virtual void ReceiveMessage(cyclus::Message::Ptr msg);

  /* -------------------- */


  /* --------------------
   * all MARKETMODEL classes have these members
   * --------------------
   */
 public:
  /**
     This instructs the market to make matches with the offers and
     requests it has on hand.
   */
  virtual void Resolve();

  /* -------------------- */


  /* --------------------
   * _THIS_ MARKETMODEL class has these members
   * --------------------
   */
 private:
  /**
     The messages that have been indexed
   */
  typedef std::pair<double, cyclus::Message::Ptr> IndexedMsg;

  /**
     The messages of both offer and request types that have been sorted
     according to thie size.
   */
  typedef std::multimap<double, cyclus::Message::Ptr> SortedMsgList;

  /**
     The requests that have been sorted according to their size.
   */
  SortedMsgList requests_;

  /**
     The offers that have been sorted according to their size.
   */
  SortedMsgList offers_;

  /**
     The set of pointers to offers that have been matched.
   */
  std::set<cyclus::Message::Ptr> matchedOffers_;

  /**
     This function adds an cyclus::Message object to the list of matchedOffers

     @param msg a pointer to the message to add
   */
  void add(cyclus::Message::Ptr msg);

  /**
     A boolean that keeps track of whether requests have been matched.
     True if matched, false if umatched.
   */
  bool match_request(SortedMsgList::iterator request);

  /**
     A boolean that keeps track of whether requests have been rejected.
     True if rejected, false if not rejected.
   */
  void reject_request(SortedMsgList::iterator request);

  /**
     This function processes the information in a request.
   */
  void process_request();

};
} // namespace cycamore
#endif