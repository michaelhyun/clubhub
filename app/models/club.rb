class Club < ActiveRecord::Base
	belongs_to :category
	belongs_to :user
	has_many :reviews
	ratyrate_rateable "rating"
end
