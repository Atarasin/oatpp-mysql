#ifndef oatpp_mysql_Executor_hpp
#define oatpp_mysql_Executor_hpp

#include "ConnectionProvider.hpp"
#include "QueryResult.hpp"
#include "mapping/Serializer.hpp"

#include "oatpp/orm/Executor.hpp"

namespace oatpp { namespace mysql {

class Executor : public orm::Executor {
private:
  /*
   * We need this invalidator to correlate abstract orm::Connection to its correct invalidator.
   */
  class ConnectionInvalidator : public provider::Invalidator<orm::Connection> {
  public:
    void invalidate(const std::shared_ptr<orm::Connection>& connection) override;
  };

private:
  std::shared_ptr<ConnectionInvalidator> m_connectionInvalidator;
  std::shared_ptr<provider::Provider<Connection>> m_connectionProvider;
  std::shared_ptr<mapping::Serializer> m_serializer;
  std::shared_ptr<mapping::ResultMapper> m_resultMapper;

private:
  struct QueryParameter {
    oatpp::String name;
    std::vector<std::string> propertyPath;
  };

  QueryParameter parseQueryParameter(const oatpp::String& paramName);

private:
  void bindParams(MYSQL_STMT* stmt,
                  const StringTemplate& queryTemplate,
                  const std::unordered_map<oatpp::String, oatpp::Void>& params,
                  const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver);

public:

  Executor(const std::shared_ptr<provider::Provider<Connection>>& connectionProvider);

  /**
   * Get default type resolver.
   * @return
   */
  std::shared_ptr<data::mapping::TypeResolver> createTypeResolver() override;

  /**
   * Get database connection.
   * @return
   */
  provider::ResourceHandle<orm::Connection> getConnection() override;

  /**
   * Parse query template.
   * @param name - template name.
   * @param text - template text.
   * @param paramsTypeMap - template parameter types.
   * @param prepare - `true` if the query should use prepared statement, `false` otherwise.
   * @return - &id:oatpp::data::share::StringTemplate;.
   */
  StringTemplate parseQueryTemplate(const oatpp::String& name,
                                    const oatpp::String& text,
                                    const ParamsTypeMap& paramsTypeMap,
                                    bool prepare = false) override;

  /**
   * Execute database query using a query template.
   * @param queryTemplate - a query template obtained in a prior call to &l:Executor::parseQueryTemplate (); method.
   * @param params - query parameters.
   * @param enabledInterpretations - enabled type interpretations.
   * @param connection - database connection.
   * @return - &id:oatpp::orm::QueryResult;.
   */
  std::shared_ptr<orm::QueryResult> execute(const StringTemplate& queryTemplate,
                                            const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                            const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver = nullptr,
                                            const provider::ResourceHandle<orm::Connection>& connection = nullptr)  override;

  /**
   * Begin database transaction. Should NOT be used directly. Use &id:oatpp::orm::Transaction; instead.
   * @param connection - database connection.
   * @return - &id:oatpp::orm::QueryResult;.
   */
  std::shared_ptr<orm::QueryResult> begin(const provider::ResourceHandle<orm::Connection>& connection = nullptr) override;

  /**
   * Commit database transaction. Should NOT be used directly. Use &id:oatpp::orm::Transaction; instead.
   * @param connection
   * @return - &id:oatpp::orm::QueryResult;.
   */
  std::shared_ptr<orm::QueryResult> commit(const provider::ResourceHandle<orm::Connection>& connection) override;

  /**
   * Rollback database transaction. Should NOT be used directly. Use &id:oatpp::orm::Transaction; instead.
   * @param connection
   * @return - &id:oatpp::orm::QueryResult;.
   */
  std::shared_ptr<orm::QueryResult> rollback(const provider::ResourceHandle<orm::Connection>& connection) override;

  /**
   * Get current database schema version.
   * @param suffix - suffix of schema version control table name.
   * @param connection - database connection.
   * @return - schema version.
   */
  v_int64 getSchemaVersion(const oatpp::String& suffix = nullptr,
                           const provider::ResourceHandle<orm::Connection>& connection = nullptr) override;

  /**
   * Run schema migration script. Should NOT be used directly. Use &id:oatpp::orm::SchemaMigration; instead.
   * @param script - script text.
   * @param newVersion - schema version corresponding to this script.
   * @param suffix - suffix of schema version control table name.
   * @param connection - database connection.
   */
  void migrateSchema(const oatpp::String& script,
                     v_int64 newVersion,
                     const oatpp::String& suffix = nullptr,
                     const provider::ResourceHandle<orm::Connection>& connection = nullptr) override;

};

}}

#endif /* oatpp_mysql_Executor_hpp */