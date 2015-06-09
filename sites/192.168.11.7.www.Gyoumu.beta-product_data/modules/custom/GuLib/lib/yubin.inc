<?php 

// 全国郵便番号一覧 - 郵便専門ネット http://yubin.senmon.net/
// PHP XML-RPCクライアントクラス yubin.class.php
// 使い方は一番下のテストを。
//
// 2002.11.05 受け渡す郵便番号の型がintになっていたのをstringに修正。
// 2002.11.06 見つからない場合にはfaultCode()が返るようになったのでそれに合わせて変更。
// 2002.11.08 セルフテストのスペルミスを訂正。
// 2002.11.09 仕様変更に対応。初期化関数の冗長な記述を削除。
// 2002.11.11 Addressというクラス名は一般的するぎるのでYubinXMLRPC_Addressに変更。
// 2005.7.1 XML-RPC for PHP
//
//
// 必要なライブラリ XML-RPC for PHP
// http://sourceforge.net/projects/phpxmlrpc/ からダウンロードして、以下のincludeを環境に合わせて書き換えてください。
//
// 配布 http://yubin.senmon.net/service/ からどうぞ。
// ライセンスはXML-RPC for PHPに準じます。
//
//
module_load_include('inc', 'GuLib', 'lib/xmlrpc');

class YubinXMLRPC_Address {
    var $yid; //yubin id
    var $data_type;
    var $postcode;
    var $jiscode;
    var $pref;
    var $city;
    var $town;
    var $other;
    var $pref_kana;
    var $city_kana;
    var $town_kana;
    var $debug;
    var $error_code;
    var $error_string;

    /**
* クラス変数の初期化
*/
    function init_vars() {
        $this->yid = 0;
        $this->data_type = "";
        $this->postcode = "";
        $this->jiscode = "";
        $this->pref = "";
        $this->city = "";
        $this->town = "";
        $this->other = "";
        $this->pref_kana = "";
        $this->city_kana = "";
        $this->town_kana = "";
        $this->error_code = false;
        $this->error_string = false;
    }

    function YubinXMLRPC_Address() {
        $this->init_vars();
    }

    function SetStruct($struct,$encoding = "UTF-8",$from_encoding="auto") {
        $val = $struct->structmem('yid');
        $this->yid = $val->scalarval();
        $val = $struct->structmem('postcode');
        $this->postcode = $val->scalarval();
        $val = $struct->structmem('data_type');
        $this->data_type = $val->scalarval();
        $val = $struct->structmem('jiscode');
        $this->jiscode = $val->scalarval();

        $val = $struct->structmem('pref');
        $this->pref = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
        $val = $struct->structmem('city');
        $this->city = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
        $val = $struct->structmem('town');
        $this->town = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
        $val = $struct->structmem('addr_name');
        $this->addr_name = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);

        $val = $struct->structmem('other');
        $this->other = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);

        $val = $struct->structmem('pref_kana');
        $this->pref_kana = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
        $val = $struct->structmem('city_kana');
        $this->city_kana = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
        $val = $struct->structmem('town_kana');
        $this->town_kana = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
        $val = $struct->structmem('addr_name_kana');
        $this->addr_name_kana = mb_convert_encoding($val->scalarval(),$encoding,$from_encoding);
    }

    function AsHTML($with_table=false,$summary=false,$border=1,$cellpadding=0,$cellspacing=0,$width='100%',$with_th = false) {
        $result = "";
        if ($with_table) {
            $result = sprintf('<table summary="%s" border="%d" cellpadding="%d" cellspacing="%d" width="%s">',$summary,$border,$cellpadding,$cellspacing,$width);
        }
        if ($with_th) {
            $result .= "\n<tr>";
            $result .= '<th>yid</th><th>data_type</th><th>postcode</th><th>jiscode</th>';
            $result .= '<th>pref</th><th>city</th><th>town</th>';
            $result .= '<th>pref_kana</th><th>city_kana</th><th>town_lana</th>';
            $result .= '<th>addr_name</th><th>addr_name_kana</th><th>other</th>';
            $result .= "</tr>\n";
        }
        $result .= sprintf('<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>',
            $this->yid,$this->data_type,$this->postcode,$this->jiscode,
            $this->pref,$this->city,$this->town,
            $this->pref_kana,$this->city_kana,$this->town_kana,
            $this->addr_name,$this->addr_name_kana,$this->other
            );
        if ($with_table) $result .= "\n</table>\n";
        return $result;
    }
}

class YubinXMLRPC_Client {
    var $uri;
    var $host;
    var $port;
    var $debug;
    var $error_code;
    var $error_string;

    /**
* クラス変数の初期化
*/
    function init_vars() {
        $this->error_code = false;
        $this->error_string = false;
    }

    /**
* コンストラクタ
*/
    function YubinXMLRPC_Client($debug = false, $host='yubin.senmon.net',$port=80,$uri='/service/xmlrpc/') {
        $this->debug = $debug;
        $this->host = $host;
        $this->port = $port;
        $this->uri = $uri;
        $this->init_vars();
    }

    function SendRequest($methodName,$params) {
        $this->init_vars();
        //クライアントを作成
        $client = new xmlrpc_client($this->uri,$this->host, $this->port);
        $client->setDebug($this->debug);
        //メッセージを作成してサーバに送信する

        $xmlrpc_params = array();
        if (is_array($params)) {
            foreach($params AS $k => $v) {
                $xmlrpc_params[] = new xmlrpcval($v , $k);
            }
            $message = new xmlrpcmsg($methodName, $xmlrpc_params);
        } else {
            $message = new xmlrpcmsg($methodName);
        }
        $result = $client->send($message);
        //レスポンスチェック
        if (!$result) return false;

        $this->error_code = $result->faultCode();
        $this->error_string = $result->faultString();
        if ($result->faultCode()) return false;
        else return $result->value();
    }

    /**
* 戻り値 全国郵便番号一覧のバージョンを返す
*/
    function GetVersion() {
        $value = $this->SendRequest('yubin.getVersion',false);
        if (!$value) return false;
        return $value->scalarval();
    }

    /**
* $postcode 検索する郵便番号。数値7桁で指定すること。
* 戻り値 存在すればtrue 存在しないか、エラーの場合は false
*/
    function PostcodeExists($postcode) {
        $value = $this->SendRequest('yubin.postcodeExists',array('string' => $postcode));
        if (!$value) return false;
        return $value->scalarval();
    }

    /**
* yubin.senmon.netにアクセス
* $postcode 検索する郵便番号。数値7桁で指定すること。
* $encoding 出力文字のエンコーディング EUC-JP,SJISなど、mbstringの受け入れる値
* 戻り値 存在すればtrue 存在しないか、エラーの場合は false
*/
    function FetchAddress($postcode,$encoding = "UTF-8", $from_encoding="auto") {
        $result = $this->SendRequest('yubin.fetchAddressByPostcode',array('string' => $postcode));
        if (!$result) return false;
        $i = 0;
        $ar_addr = array();
        $size = $result->arraysize();
        for($i=0;$i<$size;$i++) {
            $addr = new YubinXMLRPC_Address();
            $addr->SetStruct($result->arraymem($i), $encoding, $from_encoding);
            $ar_addr[] = $addr;
        }
        return $ar_addr;
    }

    /**
* 適当にHTMLにして返す
* $postcode 検索する郵便番号。数値7桁で指定すること。
* $encoding 出力文字のエンコーディング EUC-JP,SJISなど、mbstringの受け入れる値
* 戻り値 存在すればtrue 存在しないか、エラーの場合は false
*/
    function FetchAsHTML($postcode,$encoding="UTF-8",$from_encoding="auto") {
        $body = "";
        $result = $this->FetchAddress($postcode,$encoding,$from_encoding);
        if ($result) {
            foreach($result AS $v) {
                $body .= $v->AsHTML();
            }
        } else {
            $body = sprintf("<strong>Error occured.</strong><br>faultCode = %d<br>faultString = %s",$this->error_code,$this->error_string);
        }
        return $body;
    }
}

?>