#ifndef HTTPMODPIECE
#define HTTPMODPIECE
#include "MUQ/Modeling/ModPiece.h"
#include "HTTPComm.h"

namespace muq {
  namespace Modeling {

    /**
     * @brief A ModPiece connecting to an HTTP model
     * @details This ModPiece connects to a (remote or local) model via HTTP.
     * A more in-depth documentation on the underlying HTTP interface and the server-side part can
     * be found at https://github.com/UQ-Containers/testing. Several models and benchmarks
     * are also available in the form of ready-to-use containers.
     *
     * The main use case is straightforward coupling of model and UQ codes across
     * different languages and frameworks. Since this allows treating the model
     * mostly as a black box, greater separation of concerns between model and UQ
     * developers can be achieved.
     *
     * In order to set up an HTTPModPiece, you need to specify the address it is to connect to.
     * For an HTTP model running locally, it typically looks like this:
     *
     * @code{.cpp}
     * auto http_modpiece = std::make_shared<HTTPModPiece>("http://localhost:4242");
     * @endcode
     *
     * Passing additional configuration options to the model is supported through JSON structures.
     * For more examples on how to set up a JSON structure, refer to the documentation of json.hpp.
     *
     * @code{.cpp}
     * json config;
     * config["level"] = 1;
     * auto http_modpiece = std::make_shared<HTTPModPiece>("http://localhost:4242", config);
     * @endcode
     *
     * For testing purposes, you can use a test benchmark hosted by us. It implements a
     * very simple Bayesian posterior:
     *
     * @code{.cpp}
     * auto benchmark_modpiece = std::make_shared<HTTPModPiece>("http://testbenchmark.linusseelinger.de");
     * @endcode
     *
     * Beyond initialization, HTTPModPiece behaves like any other ModPiece.
     *
     * The implementation makes use of the HTTP model c++ header-only library,
     * which in turn depends on json.hpp for JSON and httplib.h for HTTP support.
     */
    class HTTPModPiece : public muq::Modeling::ModPiece {
    public:

      /**
       * @brief Construct HTTPModPiece, connecting to model server
       *
       * @param host The host adress (and possibly port) to connect to. May be local.
       * @param config Configuration parameters may be passed to the model.
       * @param headers Additional headers may be passed to the server, e.g. access tokens.
       */
      HTTPModPiece(const std::string host, json config = json(), httplib::Headers headers = httplib::Headers());

    private:

      Eigen::VectorXi read_input_size(const std::string host, const httplib::Headers& headers);

      Eigen::VectorXi read_output_size(const std::string host, const httplib::Headers& headers);

      void EvaluateImpl(muq::Modeling::ref_vector<Eigen::VectorXd> const& inputs) override;

      json config;
      ShallowModPieceClient client;
    };

  }
}

#endif
