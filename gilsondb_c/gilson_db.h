/*
 * gilson_db.h
 *
 *  Created on: 2 de abr. de 2025
 *      Author: mella
 */

#ifndef SRC_GILSON_DB_GILSON_DB_H_
#define SRC_GILSON_DB_GILSON_DB_H_

/*

Versão: 0.55 30/06/25

100% baseado no "neide_db" e "gilson"

gilson_db = banco de dados com giison
- no estilo de tabelas
- com colunas prefixadas no formato gilson
- limite de 256 colunas
- limite de string de 256 caracteres (utf-8)
- tem um tamanho de linhas predefinido
- controle de status do banco, validação e tratamento de erros em todas as operações
- toda linha terá um CRC e um identificador auto incremental
- header só modifica uma vez quando é criado a tabela
- é possível excluir/deletar um id/linha porem mantem o id original mas inativa a linha
- é possivel ativar a linha novamente e entao edita-la para reutilizar
- os pacotes de gilson são salvos no modo 'GSON_MODO_FULL'
- OFF_PACK_GILSON_DB=9  // status_id + check_ids (tipo uint32_t) + i_banco( 1byte para multi bancos)


- inicialmente sem suporte a mudança/alteração de formato de tabela ja criada
- cada 'nome de tabela' é um endereço de onde vai ser o ponto inicial na memória logo sempre vai gravar no mesma posição
- não tem tratamento de nivelamento de desgaste de setores, isto é, não escreve dinamicamente em multi setores

 */


//#define OFF_PACK_GILSON_DB	16  // status_id + check_ids + tamanho do pacote + crc (as 4 do tipo uint32_t)
#define OFF_PACK_GILSON_DB	9  // status_id + check_ids + idb (2 u32 + 1 u8)  LEMBRANDO que tamanho do pacote + crc ja estão no gilson 'GSON_MODO_FULL'
// o header do 'GSON_MODO_FULL' é: [0]=modo, [1:4]=crc, [5:6]=tamanho, [7]=tot_chaves (OFFSET_MODO_FULL) 'tamanho' soma pacote completo porem o crc é começa [5::] de 'tamanho'-5


// controle interno dos contadores do banco (cada um refere a um uint32_t do config banco), nao confundir com os 4 bytes init de cada item que indicam status do 'id'
// segue a ordem da struct 's_config_fs'!!!!
enum e_status_gilsondb
{
	egMAX_IDS_DB, 		// maximo de pacotes armazenados
	egOFF_IDS_DB,		// offset de cada pacote no banco (descontado 'OFF_INIT_DATA_DB')
	egCODE_DB,			// check code de cada banco
	egMAX_SIZE_DB,		// tamanho total em bytes que o banco ocupa
	egCHECK_IDS_DB,		// validador de pacotes com header
	egCONT_IDS_DB, 		// contagem real de itens validos
	egID_LIBRE_DB, 		// item libre
	egID_CONT_DB,		// id auto cont max
	egUSED_BYTES_DB,	// quanto do banco foi utilizado em bytes nesse momento
	egFREE_BYTES_DB,	// quanto tem libre em bytes nesse momento
};


enum e_config_gilsondb
{
	egFixedSize,		// 0=ativado cada pack de id é gravado com offset do pior caso para poder termos add/update/delete como um banco normal, 1=somente add data dinamica e invativar (invalida qualquer outro flag!)
	egAutoLoop, 		// 1=flag de configuração do auto_loop, nao retorna o erGILSONDB_LOT e add sempre no local do mais antigo 'id_cont'
	egCheckUpdID,		// 1=flag validar ou nao 'check_ids' na funcao de update id, temos um code de banco antigo ou lixo qualquer... vai add mesmo assim e assumir esse local como valido
	egCheckAddID,		// 1=flag add no lugar de inativo e válido nao muda 'id_cont', para fins de manter historico de 'id_cont' caso ja tenha um antigo

	egLENMAX			// final, até 32!!!!
};

enum e_erros_GILSONDB
{
	// vai fazer 'erGILSONDB_xxxx' onde xxxx-1000 = erro retornado
	erGILSONDB_OK = 0,
	erGILSONDB_DEL = -4000,
	erGILSONDB_LOT,  // banco lotado/cheio
	erGILSONDB_OCUPADO,
	erGILSONDB_NOMULTI,
	erGILSONDB_MODO,
	erGILSONDB_0,
	erGILSONDB_1,
	erGILSONDB_2,
	erGILSONDB_3,
	erGILSONDB_4,
	erGILSONDB_5,
	erGILSONDB_6,
	erGILSONDB_7,
	erGILSONDB_8,
	erGILSONDB_9,
	erGILSONDB_10,
	erGILSONDB_11,
	erGILSONDB_12,
	erGILSONDB_13,
	erGILSONDB_14,
	erGILSONDB_15,
	erGILSONDB_16,
	erGILSONDB_17,
	erGILSONDB_18,
	erGILSONDB_19,
	erGILSONDB_20,
	erGILSONDB_21,
	erGILSONDB_22,
	erGILSONDB_23,
	erGILSONDB_24,
	erGILSONDB_25,
	erGILSONDB_26,
	erGILSONDB_27,
	erGILSONDB_28,
	erGILSONDB_29,
	erGILSONDB_30,
	erGILSONDB_31,
	erGILSONDB_32,
	erGILSONDB_33,
	erGILSONDB_34,
	erGILSONDB_35,
	erGILSONDB_36,
	erGILSONDB_37,
	erGILSONDB_38,
	erGILSONDB_39,
	erGILSONDB_40,
	erGILSONDB_41,
	erGILSONDB_42,
	erGILSONDB_43,
	erGILSONDB_44,
	erGILSONDB_45,
	erGILSONDB_46,
	erGILSONDB_47,
	erGILSONDB_48,
	erGILSONDB_49,
	erGILSONDB_50,
	erGILSONDB_51,
	erGILSONDB_52,
	erGILSONDB_53,
	erGILSONDB_54,
	erGILSONDB_55,
	erGILSONDB_56,
	erGILSONDB_57,
	erGILSONDB_58,
	erGILSONDB_59,
};



//============================================================================================
//============================================================================================

int32_t gilsondb_init(void);


int32_t gilsondb_create_init(const uint32_t end_db, const uint32_t max_packs, const uint32_t codedb, const uint32_t max_bytes, const uint8_t *settings);
int32_t gilsondb_create_add(const uint8_t key, const uint8_t tipo1, const uint8_t tipo2, const uint16_t cont_list_a, const uint16_t cont_list_b, const uint16_t cont_list_step);
int32_t gilsondb_create_add_map(const uint16_t *map);
int32_t gilsondb_create_end(const uint32_t end_db);


int32_t gilsondb_add(const uint32_t end_db, uint8_t *data);
int32_t gilsondb_update(const uint32_t end_db, const uint32_t id, uint8_t *data);
int32_t gilsondb_del(const uint32_t end_db, const uint32_t id);
int32_t gilsondb_read_full(const uint32_t end_db, const uint32_t id, uint8_t *data, uint16_t *data_size, uint32_t *data_end, uint32_t *data_crc);
int32_t gilsondb_read_size(const uint32_t end_db, const uint32_t id, uint8_t *data, uint16_t *data_size);
int32_t gilsondb_read(const uint32_t end_db, const uint32_t id, uint8_t *data);


int32_t gilsondb_check(const uint32_t end_db, const uint32_t max_packs, const uint32_t offset_pack, const uint32_t codedb);
int32_t gilsondb_get_configs(const uint32_t end_db, const uint8_t tipo, uint32_t *config);
int32_t gilsondb_get_info(const uint32_t end_db, char *sms, const char *nome);

int32_t gilsondb_get_valids(const uint32_t end_db, uint32_t *cont_ids, uint16_t *valids);

int gilsondb_info_deep(const uint32_t end_db, const char *nome_banco);

int32_t gilsondb_encode_init(uint8_t *pack, const uint16_t size_max_pack);
int32_t gilsondb_encode_end(void);
int32_t gilsondb_encode_end_crc(uint32_t *crc);
int32_t gilsondb_encode_mapfix(const uint16_t *map, const uint8_t *valor);
int32_t gilsondb_encode_mapdin(const uint16_t *map, ...);
int32_t gilsondb_encode_mapnull(const uint16_t *map);

int32_t gilsondb_decode_init(const uint8_t *pack);
int32_t gilsondb_decode_end(void);
int32_t gilsondb_decode_end_crc(uint32_t *crc);
int32_t gilsondb_decode_map(const uint16_t *map, uint8_t *valor);
//int32_t gilsondb_decode_mapfix(const uint16_t *map, uint8_t *valor);
//int32_t gilsondb_decode_mapdin(const uint16_t *map, ...);


// para multi bancos no mesmo endereço via 'map'
int32_t gilsondb_create_multi_init(const uint32_t end_db, const uint32_t max_packs, const uint32_t codedb, const uint32_t max_bytes, const uint8_t n_bancos, const uint8_t *settings);
int32_t gilsondb_create_multi_add_map(const uint8_t i_banco, const uint8_t n_chaves, const uint16_t map[][6]);
int32_t gilsondb_create_multi_end(const uint32_t end_db);

int32_t gilsondb_multi_add(const uint32_t end_db, const uint8_t i_banco, uint8_t *data);

int32_t gilsondb_get_multi_valids(const uint32_t end_db, uint32_t *cont_ids, uint16_t *valids, uint8_t *i_bancos);
int32_t gilsondb_get_multi_ibanco_valids(const uint32_t end_db, uint32_t *cont_ids, const uint8_t ibanco, uint16_t *valids);

//============================================================================================
//============================================================================================


int32_t gilsondb_del_fixed(const uint32_t end_db, const uint32_t cont_del);

int32_t gilsondb_read_key(const uint32_t end_db, const uint32_t id, const uint8_t chave, uint8_t *data, uint8_t *valor);

#endif /* SRC_GILSON_DB_GILSON_DB_H_ */
