<?php

global $GLOBAL_YAHOO_LOGGER_DEBUG;
global $GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION;
global $GLOBAL_YAHOO_SESSION;
global $YahooConfig;

// デバッグログ出力
$GLOBAL_YAHOO_LOGGER_DEBUG = false;

//$GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION = "LOG";
$GLOBAL_YAHOO_LOGGER_DEBUG_DESTINATION = "CONSOLE";
$GLOBAL_YAHOO_SESSION = NULL;

define("OAUTH_PARAMS_IN_HEADERS", "HEADERS");
define("OAUTH_PARAMS_IN_POST_BODY", "POSTBODY");
define("OAUTH_SIGNATURE_PLAINTEXT", "PLAINTEXT");
define("OAUTH_SIGNATURE_HMAC_SHA1", "HMAC_SHA1");

define("YAHOO_OAUTH_RT_SESSION_TYPE", "YAHOO_OAUTH_RT_SESSION_TYPE");
define("YAHOO_OAUTH_AT_SESSION_TYPE", "YAHOO_OAUTH_AT_SESSION_TYPE");

$YahooConfig = array(
    "OAUTH_HOSTNAME" => "auth.login.yahoo.co.jp",
);

function _yapi_time(){
  return time() - 1000;
}

if(!class_exists("OAuthConsumer")) {
    require_once("OAuth.php");
}
require_once("YahooUtil.php");
require_once("YahooLogger.php");
require_once("YahooSession.php");
require_once("CookieSessionStore.php");
require_once("NoCookieSessionStore.php");
require_once("OAuthClient.php");
require_once("YahooAbstractApi.php");

/*
if(!class_exists("OAuthConsumer")) {
    require_once("lib/OAuth.php");
}
require_once("lib/YahooUtil.php");
require_once("lib/YahooLogger.php");
require_once("lib/YahooSession.php");
require_once("lib/CookieSessionStore.php");
require_once("lib/NoCookieSessionStore.php");
require_once("lib/OAuthClient.php");
*/
?>
