class User < ActiveRecord::Base
  has_secure_password
  attr_accessor :password
  EMAIL_REGEX = /^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$/i
  validates :username, :presence => true, :uniqueness => true, :length => { :in => 3..20 }
  validates :email, :presence => true, :uniqueness => true, :format => EMAIL_REGEX
  validates :password, :password_confirmation => true #password_confirmation attr
  validates_length_of :password, :in => 6..20, :on => :create
end
