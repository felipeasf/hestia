#include <eosiolib/eosio.hpp>


class hestia : public eosio::contract {
   public:
	hestia( account_name self )
      :contract(self),_accounts( _self, _self){}
	


void transfer(account_name from,
				account_name to,
				asset quantity,
				std::string memo)
{
	eosio_assert( from != to, "cannot transfer to self" );
	require_auth( from );
	eosio_assert( is_account( to ), "to account does not exist");
    	auto sym = quantity.symbol.name();
    	stats statstable( _self, sym );
    	const auto& st = statstable.get( sym );

    	require_recipient( from );
    	require_recipient( to );

    	eosio_assert( quantity.is_valid(), "invalid quantity" );
    	eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    	eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    	eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );
	
	sub_balance( from, quantity );
    	add_balance( to, quantity, from );

	
	if (to.valor_atual.amount >= to.valor_desejado.amount)
	{
		sub_balance(to,to.valor_atual);
		add_balance(to.criador,to.valor_atual,to.valor_atual);	
	}
}



   private:
	struct Entidade{
		account_name id;
		asset saldo;
		int8_t reputacao;
		uint64_t primary_key()const { return owner; }
	};

	struct Acao{
		account_name id;
		type_name descricao;
		asset valor_desejado;
		asset valor_atual;
		account_name criador;
		token_name *doadores;
	};

void sub_balance( account_name owner, asset value ) {
   accounts from_acnts( _self, owner );

   const auto& from = from_acnts.get( value.symbol.name(), "no balance object found" );
   eosio_assert( from.valor_atual.amount >= value.amount, "overdrawn balance" );


   if( from.valor_atual.amount == value.amount ) {
      from_acnts.erase( from );
   } else {
      from_acnts.modify( from, owner, [&]( auto& a ) {
          a.valor_atual -= value;
      });
   }
}

void add_balance( account_name owner, asset value, account_name ram_payer )
{
   accounts to_acnts( _self, owner );
   auto to = to_acnts.find( value.symbol.name() );
   if( to == to_acnts.end() ) {
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.valor_atual = value;
      });
   } else {
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.valor_atual += value;
      });
   }
}
}
EOSIO_ABI( hestia, (transfer))