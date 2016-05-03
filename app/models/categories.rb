class categories < ActiveRecord::Base
	belongs_to :clubs

#	before_save do 
	#	self.category.gsub!(/[\[\]\]\"]/ "") if attribute_present?("category")
	#end
end
