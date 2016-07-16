class AddReviewToClubs < ActiveRecord::Migration
  def change
    add_column :clubs, :reviews, :text
  end
end
