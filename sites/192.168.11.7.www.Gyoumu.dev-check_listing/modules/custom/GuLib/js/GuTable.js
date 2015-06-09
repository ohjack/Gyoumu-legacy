Drupal.behaviors.GuTableController = function(){
  ShowTooltip = function(e)
  {
  	var $this = $(this);
  	var text = $this.next('span.show-tooltip-text');
  	var posi = $this.position();
  	
  	text.show()
  		.css('top', posi.top + $this.height() + 5)
  		.css('left', posi.left);
  
  	return false;
  }
  
  HideTooltip = function(e)
  {
    var $this = $(this);
  	var text = $this.next('span.show-tooltip-text');
  	text.hide();
  }

  $('table.gu-table span.show-tooltip-text')
    .prev('span')
    .hover(ShowTooltip, HideTooltip);
};