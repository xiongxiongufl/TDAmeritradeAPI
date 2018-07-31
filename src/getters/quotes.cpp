/*
Copyright (C) 2018 Jonathon Ogden <jeog.dev@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses.
*/


#include <string>

#include "../../include/_tdma_api.h"

using namespace std;

namespace tdma {


class QuoteGetterImpl
        : public APIGetterImpl {
    std::string _symbol;

    void
    _build()
    {
        string url = URL_MARKETDATA + util::url_encode(_symbol) + "/quotes";
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

public:
    typedef QuoteGetter ProxyType;

    QuoteGetterImpl( Credentials& creds, const string& symbol )
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbol(symbol)
        {
            if( symbol.empty() )
                throw ValueException("empty symbol string");

            _build();
        }

    std::string
    get_symbol() const
    { return _symbol; }

    void
    set_symbol(const string& symbol)
    {
        if( symbol.empty() )
            throw ValueException("empty symbol");

        _symbol = symbol;
        build();
    }
};


class QuotesGetterImpl
        : public APIGetterImpl {
    std::set<std::string> _symbols;

    void
    _build()
    {
        string qstr = util::build_encoded_query_str(
            {{"symbol", util::join(_symbols,',')}}
        );
        string url = URL_MARKETDATA + "/quotes?" + qstr;
        APIGetterImpl::set_url(url);
    }

    virtual void
    build()
    { _build(); }

public:
    typedef QuotesGetter ProxyType;

    QuotesGetterImpl( Credentials& creds, const set<string>& symbols)
        :
            APIGetterImpl(creds, data_api_on_error_callback),
            _symbols(symbols)
        {
            if( symbols.empty() )
                throw ValueException("no symbols");

            _build();
        }

    std::set<std::string>
    get_symbols() const
    { return _symbols; }

    void
    set_symbols(const std::set<std::string>& symbols)
    {
        if( symbols.empty() )
            throw ValueException("empty symbols");

        _symbols = symbols;
        build();
    }
};

} /* tdma */

using namespace tdma;


int
QuoteGetter_Create_ABI( Credentials *pcreds,
                           const char* symbol,
                           QuoteGetter_C *pgetter,
                           int allow_exceptions )
{
    int err = getter_is_creatable<QuoteGetterImpl>(
        pcreds, pgetter, allow_exceptions
        );
    if( err )
        return err;

    if( !symbol ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return tdma::handle_error<tdma::ValueException>(
            "null symbol", allow_exceptions
            );
    }

    static auto meth = +[](Credentials *c, const char* s){
        return new QuoteGetterImpl(*c, s);
    };

    QuoteGetterImpl *obj;
    tie(obj, err) = CallImplFromABI(allow_exceptions, meth, pcreds, symbol);
    if( err ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert( QuoteGetter::TYPE_ID_LOW == QuoteGetter::TYPE_ID_HIGH );
    pgetter->type_id = QuoteGetter::TYPE_ID_LOW;
    return 0;
}


int
QuoteGetter_Destroy_ABI(QuoteGetter_C *pgetter, int allow_exceptions)
{
    return destroy_getter<QuoteGetterImpl>(pgetter, allow_exceptions);
}


int
QuoteGetter_GetSymbol_ABI( QuoteGetter_C *pgetter,
                              char **buf,
                              size_t *n,
                              int allow_exceptions)
{
    return GetterImplAccessor<char**>::template get<QuoteGetterImpl>(
        pgetter, &QuoteGetterImpl::get_symbol, buf, n, allow_exceptions
        );
}


int
QuoteGetter_SetSymbol_ABI( QuoteGetter_C *pgetter,
                              const char *symbol,
                              int allow_exceptions )
{
    return GetterImplAccessor<char**>::template set<QuoteGetterImpl>(
        pgetter, &QuoteGetterImpl::set_symbol, symbol, allow_exceptions
        );
}


int
QuotesGetter_Create_ABI( Credentials *pcreds,
                            const char** symbols,
                            QuotesGetter_C *pgetter,
                            int allow_exceptions )
{
    int err = getter_is_creatable<QuotesGetterImpl>(
        pcreds, pgetter, allow_exceptions
        );
    if( err )
        return err;

    if( !symbols ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return tdma::handle_error<tdma::ValueException>(
            "null symbols", allow_exceptions
            );
    }

    static auto meth = +[](Credentials *c, const char** s){
        int i = 0;
        std::set<std::string> strs;
        while( s[i] ) // TODO make safe
            strs.insert(s[i++]);
        return new QuotesGetterImpl(*c, strs);
    };

    QuotesGetterImpl *obj;
    tie(obj, err) = CallImplFromABI(allow_exceptions, meth, pcreds, symbols);
    if( err ){
        pgetter->obj = nullptr;
        pgetter->type_id = -1;
        return err;
    }

    pgetter->obj = reinterpret_cast<void*>(obj);
    assert( QuotesGetter::TYPE_ID_LOW == QuotesGetter::TYPE_ID_HIGH );
    pgetter->type_id = QuotesGetter::TYPE_ID_LOW;
    return 0;
}


int
QuotesGetter_Destroy_ABI(QuotesGetter_C *pgetter, int allow_exceptions)
{
    return destroy_getter<QuotesGetterImpl>(pgetter, allow_exceptions);
}


int
QuotesGetter_GetSymbols_ABI( QuotesGetter_C *pgetter,
                                char ***buf,
                                size_t *n,
                                int allow_exceptions)
{
    return GetterImplAccessor<char***>::template get<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::get_symbols, buf, n, allow_exceptions
        );
}


int
QuotesGetter_SetSymbols_ABI( QuotesGetter_C *pgetter,
                                const char **symbols,
                                int allow_exceptions )
{
    return GetterImplAccessor<char***>::template set<QuotesGetterImpl>(
        pgetter, &QuotesGetterImpl::set_symbols, symbols, allow_exceptions
        );
}

